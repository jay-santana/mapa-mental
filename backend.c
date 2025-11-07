#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Estruturas de dados
typedef struct No {
    int id;
    char texto[500];
    char tipo[20];
    double x;
    double y;
    struct No* proximo;
} No;

typedef struct Conexao {
    No* origem;
    No* destino;
    struct Conexao* proxima;
} Conexao;

typedef struct MapaMental {
    char tema[200];
    char imagemTema[20];
    char subtemas[1000];
    char detalhes[2000];
    char conexoes[1000];
    char cores[200];
    char tituloMapa[200];
    No* nos;
    Conexao* conexoes_lista;
} MapaMental;

// FunÃ§Ãµes auxiliares
void trim(char* str) {
    int i = strlen(str) - 1;
    while (i >= 0 && (str[i] == ' ' || str[i] == '\n' || str[i] == '\r')) {
        str[i] = '\0';
        i--;
    }
}

void splitString(const char* input, char delimiter, char*** resultado, int* count) {
    char* copia = strdup(input);
    char* token;
    int i;
    *count = 0;
    
    // Contar elementos
    token = strtok(copia, &delimiter);
    while (token != NULL) {
        (*count)++;
        token = strtok(NULL, &delimiter);
    }
    
    free(copia);
    copia = strdup(input);
    
    // Alocar memÃ³ria
    *resultado = (char**)malloc(*count * sizeof(char*));
    
    // Preencher array
    token = strtok(copia, &delimiter);
    for (i = 0; i < *count; i++) {
        (*resultado)[i] = (char*)malloc(strlen(token) + 1);
        strcpy((*resultado)[i], token);
        trim((*resultado)[i]);
        token = strtok(NULL, &delimiter);
    }
    
    free(copia);
}

// FunÃ§Ãµes de manipulaÃ§Ã£o de nÃ³s (recursividade)
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

void liberarNos(No* no) {
    if (no != NULL) {
        liberarNos(no->proximo); // Recursividade
        free(no);
    }
}

// FunÃ§Ãµes de manipulaÃ§Ã£o de conexÃµes
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

// FunÃ§Ã£o recursiva para encontrar nÃ³ por texto
// Função recursiva para encontrar nó por texto (mais precisa)
No* encontrarNoPorTexto(No* lista, const char* texto) {
    if (lista == NULL) return NULL;
    
    char textoLimpo[500];
    strcpy(textoLimpo, lista->texto);
    trim(textoLimpo);
    
    char buscaLimpo[500];
    strcpy(buscaLimpo, texto);
    trim(buscaLimpo);
    
    // Busca exata (ignorando emoji se houver)
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

// FunÃ§Ã£o para gerar HTML do mapa mental
void gerarHTMLMapaMental(MapaMental* mapa) {
    printf("Content-type: text/html\n\n");
    printf("<!DOCTYPE html>\n");
    printf("<html lang=\"pt-BR\">\n");
    printf("<head>\n");
    printf("    <meta charset=\"UTF-8\">\n");
    printf("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    printf("    <title>%s</title>\n", mapa->tituloMapa);
    printf("    <link rel=\"stylesheet\" href=\"../style.css\">\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("    <div id=\"tela-mapa\" class=\"tela ativa\">\n");
    printf("        <div class=\"container\">\n");
    printf("            <div class=\"cabecalho-mapa\">\n");
    printf("                <h1 class=\"titulo-mapa\">%s</h1>\n", mapa->tituloMapa);
    printf("                <p class=\"subtitulo-mapa\">Visualize e memorize suas ideias</p>\n");
    printf("            </div>\n");
    printf("            <div class=\"controles-mapa\">\n");
    printf("                <button id=\"btn-download\" class=\"btn-principal\">ðŸ“¥ Salvar Mapa</button>\n");
    printf("                <button id=\"btn-novo-mapa\" class=\"btn-secundario\" onclick=\"location.href='../index.html'\">ðŸ”„ Novo Mapa</button>\n");
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
    
    // Gerar conexÃµes (cÃ³digo existente permanece igual)
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
    
    // Gerar nÃ³s (cÃ³digo existente permanece igual)
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
    printf("        // Aplicar zoom inicial automaticamente\n");
	printf("        window.addEventListener('load', function() {\n");
	printf("            const areaMapa = document.getElementById('area-mapa');\n");
	printf("            const infoZoom = document.getElementById('info-zoom');\n");
	printf("            \n");
	printf("            // Zoom inicial de 70%%\n");
	printf("            areaMapa.style.transform = 'scale(0.7)';\n");
	printf("            infoZoom.textContent = 'Zoom: 70%';\n");
	printf("            \n");
	printf("            centralizarMapa();\n");
	printf("        });\n");
	printf("        \n");
    printf("        function aplicarZoom(fator) {\n");
    printf("            const areaMapa = document.getElementById('area-mapa');\n");
    printf("            const infoZoom = document.getElementById('info-zoom');\n");
    printf("            let escala = parseFloat(areaMapa.style.transform.replace('scale(', '').replace(')', '')) || 0.7;\n");
    printf("            escala *= fator;\n");
    printf("            escala = Math.max(0.3, Math.min(3, escala));\n");
    printf("            areaMapa.style.transform = `scale(${escala})`;\n");
    printf("            infoZoom.textContent = `Zoom: ${Math.round(escala * 100)}%`;\n");
    printf("        }\n");
    printf("        \n");
    printf("        function resetarZoom() {\n");
    printf("            const areaMapa = document.getElementById('area-mapa');\n");
    printf("            const infoZoom = document.getElementById('info-zoom');\n");
    printf("            areaMapa.style.transform = 'scale(0.7)';\n");
    printf("            infoZoom.textContent = 'Zoom: 70%';\n");
    printf("        }\n");
    printf("        \n");
    // NOVO: FunÃ§Ã£o para centralizar automaticamente o mapa
    printf("        function centralizarMapa() {\n");
    printf("            const areaMapa = document.getElementById('area-mapa');\n");
    printf("            const mapaMental = document.getElementById('mapa-mental-element');\n");
    printf("            if (areaMapa && mapaMental) {\n");
    printf("                // Centralizar horizontal e verticalmente\n");
   	printf("                const escala = parseFloat(areaMapa.style.transform.replace('scale(', '').replace(')', '')) || 0.7;\n");
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
    printf("        // Centralizar ao carregar a pÃ¡gina\n");
    printf("        window.addEventListener('load', function() {\n");
    printf("            centralizarMapa();\n");
    printf("        });\n");
    printf("        \n");
    printf("        // Recentrar apÃ³s zoom\n");
    printf("        document.querySelectorAll('.btn-zoom').forEach(btn => {\n");
    printf("            btn.addEventListener('click', function() {\n");
    printf("                setTimeout(centralizarMapa, 100);\n");
    printf("            });\n");
    printf("        });\n");
    printf("        \n");
    printf("        document.getElementById('btn-download').addEventListener('click', function() {\n");
    printf("            alert('Mapa mental gerado com sucesso! âœ…\\\\n\\\\nTema: %s\\\\nSubtema: %d\\\\nDetalhes: %d');\n",
           mapa->tema, mapa->subtemas[0] != '\0' ? 1 : 0, mapa->detalhes[0] != '\0' ? 1 : 0);
    printf("        });\n");
    printf("    </script>\n");
    printf("</body>\n");
    printf("</html>\n");
}

// FunÃ§Ã£o principal para processar dados e gerar mapa
void processarMapaMental(MapaMental* mapa) {
    // Processar subtemas
    char** subtemasArray = NULL;
    int numSubtemas = 0;
    int i, j;
    
    if (strlen(mapa->subtemas) > 0) {
        splitString(mapa->subtemas, ',', &subtemasArray, &numSubtemas);
    }
    
    // Processar detalhes
    char** detalhesArray = NULL;
    int numDetalhes = 0;
    
    if (strlen(mapa->detalhes) > 0) {
        splitString(mapa->detalhes, '\n', &detalhesArray, &numDetalhes);
    }
    
    // DimensÃµes base
    double larguraBase = 1400;
    double alturaBase = 1000;
    double centroX = larguraBase / 2;
    double centroY = alturaBase / 3;
    
    // Criar nÃ³ do tema central
//    char temaCompleto[300];
//    snprintf(temaCompleto, sizeof(temaCompleto), "%s %s", mapa->imagemTema, mapa->tema);
//    No* noTema = criarNo(temaCompleto, "tema", centroX, centroY);
	No* noTema = criarNo(mapa->tema, "tema", centroX, centroY);
    adicionarNo(&mapa->nos, noTema);
    
    // Criar nÃ³s dos subtemas
    double raioSubtemas = 250;
    No** nosSubtemas = NULL;
    
	if (numSubtemas > 0) {
	        nosSubtemas = (No**)malloc(numSubtemas * sizeof(No*));
	        
	        for (i = 0; i < numSubtemas; i++) {
	             double x, y;
	            
	            // Posicionamento específico para cada subtema
		        if (i == 0) {
		            // Primeiro subtema: esquerda superior
		            x = centroX - 280;
		            y = centroY - 120;
		        } else if (i == 1) {
		            // Segundo subtema: direita superior  
		            x = centroX + 280;
		            y = centroY - 120;
		        } else if (i == 2) {
		            // Terceiro subtema: esquerda inferior (MAIS À ESQUERDA)
		            x = centroX - 320;  // Mais para a esquerda
		            y = centroY + 180;  // Mais abaixo
		        } else if (i == 3) {
		            // Quarto subtema: direita inferior (MAIS À DIREITA)
		            x = centroX + 320;  // Mais para a direita
		            y = centroY + 180;  // Mais abaixo
		        } else {
		            // Para mais de 4 subtemas, usar posicionamento circular
		            double angulo = (2 * M_PI * i) / numSubtemas;
		            x = centroX + raioSubtemas * cos(angulo);
		            y = centroY + raioSubtemas * sin(angulo);
		        }
	            
	            No* noSubtema = criarNo(subtemasArray[i], "subtema", x, y);
	            adicionarNo(&mapa->nos, noSubtema);
	            nosSubtemas[i] = noSubtema;
	            adicionarConexao(&mapa->conexoes_lista, noTema, noSubtema);
	        }
	}
	
    // Criar nÃ³s dos detalhes
    
    if (numSubtemas > 0 && numDetalhes > 0) {
    	int detalhesPorSubtema = (numDetalhes + numSubtemas - 1) / numSubtemas;
        for (i = 0; i < numDetalhes; i++) {
            int subtemaIndex = i / detalhesPorSubtema;
            if (subtemaIndex >= numSubtemas) subtemaIndex = numSubtemas - 1;
            
            No* noSubtema = nosSubtemas[subtemaIndex];
            
            // Encontrar nÃ³ do subtema correspondente
            
            
            
			if (noSubtema != NULL) {
	            int indiceNoSubtema = i % detalhesPorSubtema;
	            double offsetY = 120 + (indiceNoSubtema * 100);
	            double offsetX;
	            
	            // Posicionamento específico baseado no índice do subtema
	            if (subtemaIndex == 0 || subtemaIndex == 2) {
	                // Primeiro e terceiro subtema: detalhes à ESQUERDA
	                offsetX = -220;
	            } else {
	                // Segundo e quarto subtema: detalhes à DIREITA
	                offsetX = 220;
	            }
	            
                double x = noSubtema->x + offsetX;
                double y = noSubtema->y + offsetY;
                
                No* noDetalhe = criarNo(detalhesArray[i], "detalhe", x, y);
                adicionarNo(&mapa->nos, noDetalhe);
                adicionarConexao(&mapa->conexoes_lista, noSubtema, noDetalhe);
            }
        }
    }
    
    // Processar conexÃµes manuais
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
        
        // Liberar memÃ³ria do array de conexÃµes
        for (i = 0; i < numConexoes; i++) {
            free(conexoesArray[i]);
        }
        free(conexoesArray);
    }
    
    // Liberar memÃ³ria dos arrays
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
    No* atual = lista;
    double minDistancia = 150; // Aumentei a distância mínima entre nós
    
    int ajustes = 0;
    int maxAjustes = 100; // Limite para evitar loop infinito
    
    while (atual != NULL && ajustes < maxAjustes) {
        No* outro = lista;
        int ajustou = 0;
        
        while (outro != NULL) {
            if (atual != outro) {
                double dx = atual->x - outro->x;
                double dy = atual->y - outro->y;
                double distancia = sqrt(dx * dx + dy * dy);
                
                if (distancia < minDistancia) {
                    // Ajustar posição para evitar sobreposição
                    double angulo = atan2(dy, dx);
                    if (distancia < 10) {
                        // Se estiver muito próximo, usar ângulo aleatório
                        angulo = (rand() % 360) * (M_PI / 180.0);
                    }
                    
                    double fatorAjuste = minDistancia - distancia + 20;
                    double novaX = atual->x + fatorAjuste * cos(angulo);
                    double novaY = atual->y + fatorAjuste * sin(angulo);
                    
                    // Limitar ao espaço disponível com margens maiores
                    novaX = fmax(150, fmin(1250, novaX));
                    novaY = fmax(150, fmin(850, novaY));
                    
                    atual->x = novaX;
                    atual->y = novaY;
                    ajustou = 1;
                    ajustes++;
                }
            }
            outro = outro->proximo;
        }
        
        if (!ajustou) {
            // Se não ajustou com ninguém, pode passar para o próximo
            atual = atual->proximo;
        }
        // Caso contrário, verifica novamente com as novas posições
    }
}
// FunÃ§Ã£o principal
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
    
    // Inicializar estrutura
    memset(&mapa, 0, sizeof(MapaMental));
    mapa.nos = NULL;
    mapa.conexoes_lista = NULL;
    
    // Obter dados do formulÃ¡rio
    query_string = getenv("QUERY_STRING");
    
    if (query_string != NULL) {
        char* query_copia = strdup(query_string);
        token = strtok(query_copia, "&");
        while (token != NULL) {
            igual = strchr(token, '=');
            if (igual != NULL) {
                *igual = '\0';
                chave = token;
                valor = igual + 1;
                
                // Decodificar URL encoding
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
                
                // Atribuir valores
                if (strcmp(chave, "tema") == 0) strcpy(mapa.tema, valor);
//                else if (strcmp(chave, "imagemTema") == 0) strcpy(mapa.imagemTema, valor);
                else if (strcmp(chave, "subtemas") == 0) strcpy(mapa.subtemas, valor);
                else if (strcmp(chave, "detalhes") == 0) strcpy(mapa.detalhes, valor);
                else if (strcmp(chave, "conexoes") == 0) strcpy(mapa.conexoes, valor);
//                else if (strcmp(chave, "cores") == 0) strcpy(mapa.cores, valor);
                else if (strcmp(chave, "tituloMapa") == 0) strcpy(mapa.tituloMapa, valor);
            }
            token = strtok(NULL, "&");
        }
        free(query_copia);
    }
    
    // Valores padrÃ£o
//    if (strlen(mapa.imagemTema) == 0) {
//        strcpy(mapa.imagemTema, "ðŸ§ ");
//    }
    if (strlen(mapa.tituloMapa) == 0) {
        snprintf(mapa.tituloMapa, sizeof(mapa.tituloMapa), "Mapa Mental: %s", mapa.tema);
    }
    
    // Processar e gerar mapa
    processarMapaMental(&mapa);
    gerarHTMLMapaMental(&mapa);
    
    // Liberar memÃ³ria
    liberarNos(mapa.nos);
    liberarConexoes(mapa.conexoes_lista);
    
    return 0;
}