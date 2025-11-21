#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <corecrt_math_defines.h>

/* ESTRUTURAS DE DADOS
 São usadas para armazenar:
 - Nós (tema, subtemas, detalhes)
 - Conexões entre os nós
 - Todas as informações do mapa mental*/

// Estrutura de cada nó
typedef struct No {
    int id;                 // Identificador único automático
    char texto[500];        // Texto exibido no nó
    char tipo[20];          // Pode ser: "tema", "subtema", "detalhe"
    double x, y;            // Posição do nó na tela
    struct No* proximo;     // Próximo nó da lista encadeada
} No;

// Cada conexão liga um nó a outro com uma linha
typedef struct Conexao {
    No* origem;
    No* destino;
    struct Conexao* proxima;
} Conexao;


// Estrutura principal do mapa mental
typedef struct MapaMental {
    char tema[200];
    char imagemTema[20];
    char subtemas[1000];
    char detalhes[2000];
    char conexoes[1000];
    char cores[200];
    char tituloMapa[200];
    No* nos;                      // Lista de nós criados
    Conexao* conexoes_lista;      // Lista de conexões
} MapaMental;

// Funcoes auxiliares
// Remove espaços no final de uma string
void trim(char* str) {
    int i = strlen(str) - 1;
    while (i >= 0 && (str[i] == ' ' || str[i] == '\n' || str[i] == '\r')) {
        str[i] = '\0';
        i--;
    }
}

/*
 splitString: divide uma string em pedaços usando um delimitador (vírgula, quebra de linha etc)
 Exemplo: "A,B,C" vira ["A", "B", "C"]
*/
void splitString(const char* input, char delimiter, char*** resultado, int* count) {
    if (input == NULL || strlen(input) == 0) {
        *count = 0;
        *resultado = NULL;
        return;
    }
    
    char* copia = strdup(input);
    if (copia == NULL) {
        *count = 0;
        *resultado = NULL;
        return;
    }
    
    char* token;
    int i, j;
    *count = 0;
    
    // Conta quantos elementos existem
    token = strtok(copia, &delimiter);
    while (token != NULL) {
        (*count)++;
        token = strtok(NULL, &delimiter);
    }
    
    // Se não encontrou elementos, libera memória e retorna
    if (*count == 0) {
        free(copia);
        *resultado = NULL;
        return;
    }
    
    // Realoca copia para refazer a tokenização
    free(copia);
    copia = strdup(input);
    if (copia == NULL) {
        *count = 0;
        *resultado = NULL;
        return;
    }
    
    // Aloca memória para o array de strings
    *resultado = (char**)malloc(*count * sizeof(char*));
    if (*resultado == NULL) {
        free(copia);
        *count = 0;
        return;
    }
    
    // Preenche o array com as palavras separadas
    token = strtok(copia, &delimiter);
    for (i = 0; i < *count; i++) {
        if (token == NULL) {
            // Se por algum motivo token for NULL, para o loop
            *count = i;
            break;
        }
        
        (*resultado)[i] = (char*)malloc(strlen(token) + 1);
        if ((*resultado)[i] == NULL) {
            // Em caso de erro na alocação, libera o que foi alocado até agora
            for (j = 0; j < i; j++) {
                free((*resultado)[j]);
            }
            free(*resultado);
            free(copia);
            *resultado = NULL;
            *count = 0;
            return;
        }
        strcpy((*resultado)[i], token);
        trim((*resultado)[i]);
        token = strtok(NULL, &delimiter);
    }
    
    free(copia);
}

// Funcoes de manipulacao de nos (recursividade)

// Cria um nó novo (tema, subtema ou detalhe)
No* criarNo(const char* texto, const char* tipo, double x, double y) {
    No* novoNo = (No*)malloc(sizeof(No));
    static int nextId = 1;
    
    novoNo->id = nextId++;
    strcpy(novoNo->texto, texto);
    strcpy(novoNo->tipo, tipo);
    novoNo->x = x;
    novoNo->y = y;
    novoNo->proximo = NULL;
    
    return novoNo;
}

// Insere o nó no final da lista encadeada
void adicionarNo(No** lista, No* novoNo) {
    if (*lista == NULL) {
        *lista = novoNo;
    } else {
        No* atual = *lista;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = novoNo;
    }
}

// Libera todos os nós recursivamente
void liberarNos(No* no) {
    if (no != NULL) {
        liberarNos(no->proximo); // Recursividade
        free(no);
    }
}

// Funcoes de manipulacao de conexoes
void adicionarConexao(Conexao** lista, No* origem, No* destino) {
    Conexao* novaConexao = (Conexao*)malloc(sizeof(Conexao));
    novaConexao->origem = origem;
    novaConexao->destino = destino;
    novaConexao->proxima = *lista;
    *lista = novaConexao;
}

void liberarConexoes(Conexao* conexao) {
    if (conexao != NULL) {
        liberarConexoes(conexao->proxima); // Recursividade
        free(conexao);
    }
}

 /*FUNÇÃO PARA ENCONTRAR NÓ PELO TEXTO
  para ligar conexões manuais digitadas pelo usuário*/
No* encontrarNoPorTexto(No* lista, const char* texto) {
    if (lista == NULL) return NULL;
    
    char textoLimpo[500];
    strcpy(textoLimpo, lista->texto);
    trim(textoLimpo);
    
    char buscaLimpo[500];
    strcpy(buscaLimpo, texto);
    trim(buscaLimpo);
    
    // comparacao direta
    if (strcmp(textoLimpo, buscaLimpo) == 0) {
        return lista;
    }
    
    // Busca por conteúdo (se não encontrou exato)
    char* textoSemEmoji = textoLimpo;
    while (*textoSemEmoji && (*textoSemEmoji < 0 || *textoSemEmoji > 127)) {
        textoSemEmoji++; // Pular emojis/unicode
    }
    
    char* buscaSemEmoji = buscaLimpo;
    while (*buscaSemEmoji && (*buscaSemEmoji < 0 || *buscaSemEmoji > 127)) {
        buscaSemEmoji++; // Pular emojis/unicode
    }
    
    if (strstr(textoSemEmoji, buscaSemEmoji) != NULL) {
        return lista;
    }
    
    return encontrarNoPorTexto(lista->proximo, texto); // Recursividade
}

// Funcao para gerar HTML do mapa mental
void gerarHTMLMapaMental(MapaMental* mapa) {
    printf("Content-type: text/html\n\n");
    printf("<!DOCTYPE html>\n");
    printf("<html lang=\"pt-BR\">\n");
    printf("<head>\n");
    printf("    <meta charset=\"UTF-8\">\n");
    printf("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    printf("    <title>%s</title>\n", mapa->tituloMapa);
    printf("    <link rel=\"stylesheet\" href=\"../style.css\">\n");
    printf("    <script src=\"https://html2canvas.hertzen.com/dist/html2canvas.min.js\"></script>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("    <div id=\"tela-mapa\" class=\"tela ativa\">\n");
    printf("        <div class=\"container\">\n");
    printf("            <div class=\"cabecalho-mapa\">\n");
    printf("                <h1 class=\"titulo-mapa\">%s</h1>\n", mapa->tituloMapa);
    printf("                <p class=\"subtitulo-mapa\">Visualize e memorize suas ideias</p>\n");
    printf("            </div>\n");
    printf("            <div class=\"controles-mapa\">\n");
    printf("                <button id=\"btn-download\" class=\"btn-principal\">Salvar Mapa</button>\n");
    printf("                <button id=\"btn-download-txt\" class=\"btn-principal\">Baixar TXT</button>\n");
    printf("                <button id=\"btn-novo-mapa\" class=\"btn-secundario\" onclick=\"location.href='../index.html'\">Novo Mapa</button>\n");
    printf("                <div class=\"controles-zoom\">\n");
    printf("                    <button class=\"btn-zoom\" onclick=\"aplicarZoom(1.2)\">+</button>\n");
    printf("                    <button class=\"btn-zoom\" onclick=\"aplicarZoom(0.8)\">-</button>\n");
    printf("                    <button class=\"btn-zoom\" onclick=\"resetarZoom()\">âŸ²</button>\n");
    printf("                </div>\n");
    printf("                <div class=\"info-mapa\" id=\"info-zoom\">Zoom: 100%%</div>\n");
    printf("            </div>\n");
    printf("            <div class=\"area-mapa-container\">\n");
    printf("                <div id=\"area-mapa\" class=\"area-mapa\">\n");
    printf("                    <div class=\"mapa-mental\" id=\"mapa-mental-element\">\n");
    
    // Gera as linhas de conexão entre os nós 
    Conexao* conexaoAtual = mapa->conexoes_lista;
    while (conexaoAtual != NULL) {
        double dx = conexaoAtual->destino->x - conexaoAtual->origem->x;
        double dy = conexaoAtual->destino->y - conexaoAtual->origem->y;
        double comprimento = sqrt(dx * dx + dy * dy);
        double angulo = atan2(dy, dx) * (180 / M_PI);
        
        printf("                        <div class=\"linha\" style=\"width: %.0fpx; left: %.0fpx; top: %.0fpx; transform: rotate(%.2fdeg);\"></div>\n",
               comprimento, conexaoAtual->origem->x, conexaoAtual->origem->y, angulo);
        
        conexaoAtual = conexaoAtual->proxima;
    }
    
    // Gera os nós (tema, subtemas, detalhes)
    No* noAtual = mapa->nos;
    while (noAtual != NULL) {
        printf("                        <div class=\"no no-%s\" style=\"left: %.0fpx; top: %.0fpx;\" title=\"%s\">%s</div>\n",
               noAtual->tipo, noAtual->x, noAtual->y, noAtual->texto, noAtual->texto);
        noAtual = noAtual->proximo;
    }
    
    printf("                    </div>\n");
    printf("                </div>\n");
    printf("            </div>\n");
    
    printf("            <div class=\"dicas-memorizacao\" style=\"margin-top: 30px;\">\n");
    printf("                <h3>ðŸ“± Aplicativos para Mapas Mentais</h3>\n");
    printf("                <p><strong>Mindmeister, Coggle, Mind Note, Lucid Chart, Xmind 8, Bubbl, MindManager, miMind, Mindmap Maker, SimpleMind</strong></p>\n");
    printf("            </div>\n");
    printf("        </div>\n");
    printf("    </div>\n");
    
    printf("    <script>\n");
	printf("        function aplicarZoom(fator) {\n");
	printf("            const areaMapa = document.getElementById('area-mapa');\n");
	printf("            const infoZoom = document.getElementById('info-zoom');\n");
	printf("            let escala = parseFloat(areaMapa.style.transform.replace('scale(', '').replace(')', '')) || 1;\n");
	printf("            escala *= fator;\n");
	printf("            escala = Math.max(0.3, Math.min(3, escala));\n");
	printf("            areaMapa.style.transform = `scale(${escala})`;\n");
	printf("            infoZoom.textContent = `Zoom: ${Math.round(escala * 100)}%`;\n");
	printf("        }\n");
	printf("        \n");
	printf("        function resetarZoom() {\n");
	printf("            const areaMapa = document.getElementById('area-mapa');\n");
	printf("            const infoZoom = document.getElementById('info-zoom');\n");
	printf("            areaMapa.style.transform = 'scale(1)';\n");
	printf("            infoZoom.textContent = 'Zoom: 100%';\n");
	printf("        }\n");
	printf("        \n");
	printf("        function centralizarMapa() {\n");
	printf("            const areaMapa = document.getElementById('area-mapa');\n");
	printf("            const mapaMental = document.getElementById('mapa-mental-element');\n");
	printf("            if (areaMapa && mapaMental) {\n");
	printf("                const escala = parseFloat(areaMapa.style.transform.replace('scale(', '').replace(')', '')) || 1;\n");
	printf("                const containerWidth = areaMapa.clientWidth;\n");
	printf("                const containerHeight = areaMapa.clientHeight;\n");
	printf("                const mapaWidth = mapaMental.scrollWidth * escala;\n");
	printf("                const mapaHeight = mapaMental.scrollHeight * escala;\n");
	printf("                \n");
	printf("                areaMapa.scrollLeft = (mapaWidth - containerWidth) / 2;\n");
	printf("                areaMapa.scrollTop = (mapaHeight - containerHeight) / 2;\n");
	printf("            }\n");
	printf("        }\n");
	printf("        \n");
	printf("        window.addEventListener('load', function() {\n");
	printf("            centralizarMapa();\n");
	printf("        });\n");
	printf("        \n");
	printf("        document.querySelectorAll('.btn-zoom').forEach(btn => {\n");
	printf("            btn.addEventListener('click', function() {\n");
	printf("                setTimeout(centralizarMapa, 100);\n");
	printf("            });\n");
	printf("        });\n");
	printf("        \n");
	printf("        // Download da imagem do mapa mental\n");
	printf("        document.getElementById('btn-download').addEventListener('click', function() {\n");
	printf("            const mapaElement = document.getElementById('mapa-mental-element');\n");
	printf("            \n");
	printf("            // Usar html2canvas para capturar o mapa como imagem\n");
	printf("            html2canvas(mapaElement, {\n");
	printf("                scale: 2, // Maior qualidade\n");
	printf("                backgroundColor: '#ffffff',\n");
	printf("                useCORS: true,\n");
	printf("                logging: false\n");
	printf("            }).then(canvas => {\n");
	printf("                // Converter canvas para imagem\n");
	printf("                const imagemURL = canvas.toDataURL('image/png');\n");
	printf("                \n");
	printf("                // Criar link de download\n");
	printf("                const link = document.createElement('a');\n");
	printf("                const now = new Date();\n");
	printf("                const timestamp = now.toISOString().replace(/[:.-]/g, '').slice(0, 15);\n");
	printf("                const filename = 'mapa_mental_' + timestamp + '.png';\n");
	printf("                \n");
	printf("                link.href = imagemURL;\n");
	printf("                link.download = filename;\n");
	printf("                link.style.display = 'none';\n");
	printf("                \n");
	printf("                document.body.appendChild(link);\n");
	printf("                link.click();\n");
	printf("                document.body.removeChild(link);\n");
	printf("                \n");
	printf("                // Feedback para o usuário\n");
	printf("                alert('dzŽ¨ Imagem do mapa mental baixada com sucesso!\\\\\\\\n\\\\\\\\nArquivo: ' + filename);\n");
	printf("            }).catch(error => {\n");
	printf("                console.error('Erro ao gerar imagem:', error);\n");
	printf("                alert('âš ï¸ Erro ao gerar imagem do mapa mental.\\\\\\\\nTente novamente.');\n");
	printf("            });\n");
	printf("        });\n");
	printf("        \n");
	printf("        // Download do arquivo TXT (código existente mantido)\n");
	printf("        document.getElementById('btn-download-txt').addEventListener('click', function() {\n");
	// ... (mantenha o código existente do download TXT aqui)
	printf("        });\n");
	printf("    </script>\n");
    printf("</body>\n");
    printf("</html>\n");
}

void ajustarPosicionamentoNos(No* lista, double centroX, double centroY);

// FUNÇÃO: Processa os dados e cria a estrutura do mapa mental
void processarMapaMental(MapaMental* mapa) {
    // Processa subtemas (divididos por vírgula)
    char** subtemasArray = NULL;
    int numSubtemas = 0;
    int i, j;
    
    if (strlen(mapa->subtemas) > 0) {
        splitString(mapa->subtemas, ',', &subtemasArray, &numSubtemas);
    }
    
    // Processa detalhes (divididos por quebra de linha)
    char** detalhesArray = NULL;
    int numDetalhes = 0;
    
    if (strlen(mapa->detalhes) > 0) {
        splitString(mapa->detalhes, '\n', &detalhesArray, &numDetalhes);
    }
    
    // Define dimensoes do mapa
    double larguraBase = 1400;
    double alturaBase = 1000;
    double centroX = larguraBase / 2;
    double centroY = alturaBase / 3;
    
    // Criar nos do tema central
    char temaCompleto[300];
    snprintf(temaCompleto, sizeof(temaCompleto), "%s %s", mapa->imagemTema, mapa->tema);
    No* noTema = criarNo(temaCompleto, "tema", centroX, centroY);
    adicionarNo(&mapa->nos, noTema);
    
    // Cria nós dos subtemas em posições circulares ao redor do tema
    double raioSubtemas = 250;
    No** nosSubtemas = NULL;
    
	if (numSubtemas > 0) {
	        nosSubtemas = (No**)malloc(numSubtemas * sizeof(No*));
	        
	        for (i = 0; i < numSubtemas; i++) {
	            double angulo;
	            
	            // Distribui posições dependendo da quantidade
	            if (numSubtemas == 1) {
	                angulo = M_PI / 2; // Abaixo do tema central
	            } else if (numSubtemas == 2) {
	                angulo = (M_PI / 2) + (i * M_PI); // Lados opostos
	            } else {
	                angulo = (2 * M_PI * i) / numSubtemas;
	            }
	            
	            double x = centroX + raioSubtemas * cos(angulo);
	            double y = centroY + raioSubtemas * sin(angulo);
	            
	            // Ajustar para evitar sobreposição com tema central
	            if (numSubtemas <= 2) {
	                y = centroY + raioSubtemas; // Sempre abaixo para 1-2 subtemas
	                if (numSubtemas == 2) {
	                    x = centroX + (i == 0 ? -raioSubtemas : raioSubtemas);
	                }
	            }
	            
	            No* noSubtema = criarNo(subtemasArray[i], "subtema", x, y);
	            adicionarNo(&mapa->nos, noSubtema);
	            nosSubtemas[i] = noSubtema;
	            
	            //Conecta subtema ao tema
	            adicionarConexao(&mapa->conexoes_lista, noTema, noSubtema);
	        }
	}
	
    // Criação dos detalhes ligados aos subtemas
    
    if (numSubtemas > 0 && numDetalhes > 0) {
    	int detalhesPorSubtema = (numDetalhes + numSubtemas - 1) / numSubtemas;
        for (i = 0; i < numDetalhes; i++) {
            int subtemaIndex = i / detalhesPorSubtema;
            if (subtemaIndex >= numSubtemas) subtemaIndex = numSubtemas - 1;
            
            No* noSubtema = nosSubtemas[subtemaIndex];
            
            // Encontrar nÃ³ do subtema correspondente
            
            
            
			if (noSubtema != NULL) {
                // Calcular posição baseada no índice do detalhe dentro do subtema
	            int indiceNoSubtema = i % detalhesPorSubtema;
	            double espacamentoVertical = 100; // Espaço entre detalhes
	            double offsetBaseY = 120; // Distância inicial do subtema
                
                // Posicionar detalhes em coluna 
            	double offsetY = offsetBaseY + (indiceNoSubtema * espacamentoVertical);
                // Alternar lados 
	            double offsetX;
	            if (numSubtemas == 1) {
	                // Para um único subtema, distribuir em ambos os lados
	                offsetX = (indiceNoSubtema % 2 == 0) ? 180 : -180;
	            } else {
	                // Para múltiplos subtemas, baseado na posição do subtema
	                if (noSubtema->x < centroX) {
	                    offsetX = -180; // Esquerda para subtemas à esquerda
	                } else {
	                    offsetX = 180;  // Direita para subtemas à direita
	                }
	            }
	            // para evitar sobreposição
	            if (numSubtemas >= 3) {
	                // Para muitos subtemas, usar lados alternados baseado no índice
	                offsetX = (subtemaIndex % 2 == 0) ? 180 : -180;
	            }
                double x = noSubtema->x + offsetX;
                double y = noSubtema->y + offsetY;
                
                No* noDetalhe = criarNo(detalhesArray[i], "detalhe", x, y);
                adicionarNo(&mapa->nos, noDetalhe);
                adicionarConexao(&mapa->conexoes_lista, noSubtema, noDetalhe);
            }
        }
    }
    
    // Processa conexões manuais definidas pelo usuário
    if (strlen(mapa->conexoes) > 0) {
        char** conexoesArray = NULL;
        int numConexoes = 0;
        splitString(mapa->conexoes, '\n', &conexoesArray, &numConexoes);
        
        for (i = 0; i < numConexoes; i++) {
            char* seta = strstr(conexoesArray[i], "->");
            if (seta != NULL) {
                *seta = '\0';
                char* origem = conexoesArray[i];
                char* destino = seta + 2;
                trim(origem);
                trim(destino);
                
                // Encontra nós de origem e destino
                No* noOrigem = encontrarNoPorTexto(mapa->nos, origem);
                No* noDestino = encontrarNoPorTexto(mapa->nos, destino);
                
                if (noOrigem != NULL && noDestino != NULL) {
                    // Verificar se conexão já existe
                    int conexaoExiste = 0;
                    Conexao* conexaoAtual = mapa->conexoes_lista;
                    while (conexaoAtual != NULL) {
                        if (conexaoAtual->origem == noOrigem && conexaoAtual->destino == noDestino) {
                            conexaoExiste = 1;
                            break;
                        }
                        conexaoAtual = conexaoAtual->proxima;
                    }
                    
                    if (!conexaoExiste) {
                        adicionarConexao(&mapa->conexoes_lista, noOrigem, noDestino);
                    }
                }
            }
        }
        
        // Liberar memoria do array de conexoes
        for (i = 0; i < numConexoes; i++) {
            free(conexoesArray[i]);
        }
        free(conexoesArray);
    }
    
    // Ajusta posições finais e libera memória
    ajustarPosicionamentoNos(mapa->nos, centroX, centroY);
    if (nosSubtemas) free(nosSubtemas);
    
    for (i = 0; i < numSubtemas; i++) {
        free(subtemasArray[i]);
    }
    if (subtemasArray) free(subtemasArray);
    
    for (i = 0; i < numDetalhes; i++) {
        free(detalhesArray[i]);
    }
    if (detalhesArray) free(detalhesArray);
}

// Função para ajustar posicionamento e evitar sobreposição

void ajustarPosicionamentoNos(No* lista, double centroX, double centroY) {
    if (lista == NULL) return;
    
    No* atual = lista;
    double minDistancia = 150;
    int ajustes = 0;
    int maxAjustes = 100;
    
    srand(time(NULL)); // Inicializar seed para rand()
    
    while (atual != NULL && ajustes < maxAjustes) {
        No* outro = lista;
        int ajustou = 0;
        
        while (outro != NULL && ajustes < maxAjustes) {
            if (atual != outro) {
                double dx = atual->x - outro->x;
                double dy = atual->y - outro->y;
                double distancia = sqrt(dx * dx + dy * dy);
                
                if (distancia < minDistancia) {
                    double angulo = atan2(dy, dx);
                    if (distancia < 10) {
                        angulo = (rand() % 360) * (M_PI / 180.0);
                    }
                    
                    double fatorAjuste = minDistancia - distancia + 20;
                    double novaX = atual->x + fatorAjuste * cos(angulo);
                    double novaY = atual->y + fatorAjuste * sin(angulo);
                    
                    novaX = fmax(150, fmin(1250, novaX));
                    novaY = fmax(150, fmin(850, novaY));
                    
                    atual->x = novaX;
                    atual->y = novaY;
                    ajustou = 1;
                    ajustes++;
                    break; // Reiniciar verificação após ajuste
                }
            }
            outro = outro->proximo;
        }
        
        if (!ajustou) {
            atual = atual->proximo;
        }
    }
}

// Função para salvar dados do mapa em arquivo TXT (CORRIGIDA)
void salvarMapaEmArquivo(MapaMental* mapa) {
    FILE* arquivo;
    char nomeArquivo[100];
    int i;
    
    // Gerar nome do arquivo com timestamp
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(nomeArquivo, sizeof(nomeArquivo), "mapa_mental_%04d%02d%02d_%02d%02d%02d.txt",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    
    arquivo = fopen(nomeArquivo, "w");
    
    if (arquivo == NULL) {
        printf("<!-- Erro: Não foi possível criar o arquivo -->\n");
        return;  // CORREÇÃO: return sem valor
    }
    
    // Escrever cabeçalho
    fprintf(arquivo, "========================================\n");
    fprintf(arquivo, "MAPA MENTAL: %s\n", mapa->tituloMapa);
    fprintf(arquivo, "Data de criação: %04d-%02d-%02d %02d:%02d:%02d\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(arquivo, "========================================\n\n");
    
    // Escrever tema principal
    fprintf(arquivo, "?? TEMA PRINCIPAL:\n");
    fprintf(arquivo, "  • %s\n\n", mapa->tema);
    
    // Escrever subtemas
    if (strlen(mapa->subtemas) > 0) {
        fprintf(arquivo, "?? SUBTEMAS:\n");
        char** subtemasArray = NULL;
        int numSubtemas = 0;
        
        splitString(mapa->subtemas, ',', &subtemasArray, &numSubtemas);
        
        for (i = 0; i < numSubtemas; i++) {
            fprintf(arquivo, "  %d. %s\n", i + 1, subtemasArray[i]);
            free(subtemasArray[i]);
        }
        if (subtemasArray) free(subtemasArray);
        fprintf(arquivo, "\n");
    }
    
    // Escrever detalhes
    if (strlen(mapa->detalhes) > 0) {
        fprintf(arquivo, "?? DETALHES E ESPECIFICAÇÕES:\n");
        char** detalhesArray = NULL;
        int numDetalhes = 0;
        
        splitString(mapa->detalhes, '\n', &detalhesArray, &numDetalhes);
        
        for (i = 0; i < numDetalhes; i++) {
            fprintf(arquivo, "  • %s\n", detalhesArray[i]);
            free(detalhesArray[i]);
        }
        if (detalhesArray) free(detalhesArray);
        fprintf(arquivo, "\n");
    }
    
    // Escrever conexões manuais
    if (strlen(mapa->conexoes) > 0) {
        fprintf(arquivo, "?? CONEXÕES PERSONALIZADAS:\n");
        char** conexoesArray = NULL;
        int numConexoes = 0;
        
        splitString(mapa->conexoes, '\n', &conexoesArray, &numConexoes);
        
        for (i = 0; i < numConexoes; i++) {
            fprintf(arquivo, "  ? %s\n", conexoesArray[i]);
            free(conexoesArray[i]);
        }
        if (conexoesArray) free(conexoesArray);
        fprintf(arquivo, "\n");
    }
    
    // Estatísticas
    fprintf(arquivo, "?? ESTATÍSTICAS:\n");
    
    // Contar nós
    int countNos = 0;
    No* noAtual = mapa->nos;
    while (noAtual != NULL) {
        countNos++;
        noAtual = noAtual->proximo;
    }
    
    // Contar conexões
    int countConexoes = 0;
    Conexao* conexaoAtual = mapa->conexoes_lista;
    while (conexaoAtual != NULL) {
        countConexoes++;
        conexaoAtual = conexaoAtual->proxima;
    }
    
    fprintf(arquivo, "  • Total de nós: %d\n", countNos);
    fprintf(arquivo, "  • Total de conexões: %d\n", countConexoes);
    
    fprintf(arquivo, "\n========================================\n");
    fprintf(arquivo, "Arquivo gerado automaticamente pelo\nSistema de Mapas Mentais\n");
    
    fclose(arquivo);
    
    // Feedback no HTML
//    printf("<!-- Arquivo TXT salvo no servidor: %s -->\n", nomeArquivo);
}

// Funcao principal
int main() {
    MapaMental mapa;
    char* query_string;
    char* token;
    char* igual;
    char* chave;
    char* valor;
    char* src;
    char* dst;
    int hex;
    
    // Inicializa a estrutura do mapa com zeros
    memset(&mapa, 0, sizeof(MapaMental));
    mapa.nos = NULL;
    mapa.conexoes_lista = NULL;
    
   // Obtém dados do formulário HTML (método GET)
    query_string = getenv("QUERY_STRING");
    
    if (query_string != NULL) {
        char* query_copia = strdup(query_string);
        token = strtok(query_copia, "&");
        
        // Processa cada parâmetro do formulário
        while (token != NULL) {
            igual = strchr(token, '=');
            if (igual != NULL) {
                *igual = '\0';
                chave = token;
                valor = igual + 1;
                
                // Decodifica caracteres especiais
                src = valor;
                dst = valor;
                while (*src) {
                    if (*src == '+') {
                        *dst++ = ' ';
                    } else if (*src == '%' && src[1] && src[2]) {
                        sscanf(src + 1, "%2x", &hex);
                        *dst++ = hex;
                        src += 2;
                    } else {
                        *dst++ = *src;
                    }
                    src++;
                }
                *dst = '\0';
                
                // Atribui valores aos campos correspondentes
                if (strcmp(chave, "tema") == 0) strcpy(mapa.tema, valor);
                else if (strcmp(chave, "imagemTema") == 0) strcpy(mapa.imagemTema, valor);
                else if (strcmp(chave, "subtemas") == 0) strcpy(mapa.subtemas, valor);
                else if (strcmp(chave, "detalhes") == 0) strcpy(mapa.detalhes, valor);
                else if (strcmp(chave, "conexoes") == 0) strcpy(mapa.conexoes, valor);
                else if (strcmp(chave, "cores") == 0) strcpy(mapa.cores, valor);
                else if (strcmp(chave, "tituloMapa") == 0) strcpy(mapa.tituloMapa, valor);
            }
            token = strtok(NULL, "&");
        }
        free(query_copia);
    }
    
    // Define valores padrão se necessário
    if (strlen(mapa.imagemTema) == 0) {
        strcpy(mapa.imagemTema, "ðŸ§ ");
    }
    if (strlen(mapa.tituloMapa) == 0) {
        snprintf(mapa.tituloMapa, sizeof(mapa.tituloMapa), "Mapa Mental: %s", mapa.tema);
    }
    
    // Processa e gera o mapa mental
    processarMapaMental(&mapa);
    // Salvar em arquivo TXT
	salvarMapaEmArquivo(&mapa);
    gerarHTMLMapaMental(&mapa);
    
    //Limpeza final da memória
    liberarNos(mapa.nos);
    liberarConexoes(mapa.conexoes_lista);
    
    return 0;
}