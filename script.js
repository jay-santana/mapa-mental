
document.addEventListener('DOMContentLoaded', function () {
    // Elementos da interface
    const telaInicial = document.getElementById('tela-inicial');
    const telaOpcoes = document.getElementById('tela-opcoes');
    const popupInfo = document.getElementById('popup-info');
    const btnIniciar = document.getElementById('btn-iniciar');
    const btnVoltarInicio = document.getElementById('btn-voltar-inicio');
    const btnFecharPopup = document.querySelector('.fechar');
    const btnAnterior = document.getElementById('btn-anterior');
    const btnProximo = document.getElementById('btn-proximo');
    const etapasForm = document.querySelectorAll('.etapa-form');
    const etapasOpcoes = document.querySelectorAll('.etapa[data-etapa]');

    let etapaAtual = 1;
    const totalEtapas = 3; // Reduzido para 3 etapas
    let nosColetados = [];

    // Navegação entre telas
    btnIniciar.addEventListener('click', function () {
        telaInicial.classList.remove('ativa');
        telaOpcoes.classList.add('ativa');
    });

    btnVoltarInicio.addEventListener('click', function () {
        telaOpcoes.classList.remove('ativa');
        telaInicial.classList.add('ativa');
    });

    // Controle do popup
    etapasOpcoes.forEach(etapa => {
        etapa.addEventListener('click', function () {
            if (!this.classList.contains('bloqueada')) {
                etapaAtual = parseInt(this.dataset.etapa);
                abrirPopup();
            }
        });
    });

    btnFecharPopup.addEventListener('click', fecharPopup);

    // Navegação no popup
    btnAnterior.addEventListener('click', function () {
        if (etapaAtual > 1) {
            etapaAtual--;
            atualizarPopup();
        }
    });

    btnProximo.addEventListener('click', function () {
        if (etapaAtual === 1) {
            // Etapa do tema central - validar e avançar
            if (!document.getElementById('tema').value.trim()) {
                alert('Por favor, insira o tema central.');
                return;
            }
            etapaAtual++;
            atualizarPopup();
        } else if (etapaAtual === 2) {
            // Etapa dos nós - coletar dados do nó atual
            coletarNo();
        } else {
            // Última etapa - submeter formulário
            prepararFormularioFinal();
            document.getElementById('form-info').submit();
        }
    });

    function coletarNo() {
        const subtema = document.getElementById('subtema').value.trim();
        const detalhes = document.getElementById('detalhesNo').value.trim();
        const conexoes = document.getElementById('conexoesNo').value.trim();

        if (!subtema) {
            alert('Por favor, insira pelo menos o subtema.');
            return;
        }

        // Adicionar aos nós coletados
        const no = {
            subtema: subtema,
            detalhes: detalhes.split('\n').filter(d => d.trim() !== ''),
            conexoes: conexoes.split('\n').filter(c => c.trim() !== '')
        };
        
        nosColetados.push(no);

        // Limpar campos
        document.getElementById('subtema').value = '';
        document.getElementById('detalhesNo').value = '';
        document.getElementById('conexoesNo').value = '';

        // Perguntar se deseja adicionar mais nós
        const adicionarMais = confirm(`Nó "${subtema}" adicionado com sucesso!\n\nDeseja adicionar outro nó?`);
        
        if (!adicionarMais) {
            etapaAtual++;
            atualizarPopup();
        } else {
            // Manter na mesma etapa para adicionar mais nós
            document.getElementById('subtema').focus();
        }
    }

    function prepararFormularioFinal() {
        // Coletar tema central
        const tema = document.getElementById('tema').value.trim();
        
        // Juntar todos os subtemas em uma string
        const todosSubtemas = nosColetados.map(no => no.subtema).join(', ');
        document.getElementById('subtemas').value = todosSubtemas;
        
        // Juntar todos os detalhes em uma string
        const todosDetalhes = nosColetados.flatMap(no => no.detalhes).join('\n');
        document.getElementById('detalhes').value = todosDetalhes;
        
        // Juntar todas as conexões em uma string
        const todasConexoes = nosColetados.flatMap(no => no.conexoes).join('\n');
        document.getElementById('conexoes').value = todasConexoes;
        
        // Definir título do mapa se não foi preenchido
        if (!document.getElementById('tituloMapa').value.trim()) {
            document.getElementById('tituloMapa').value = `Mapa Mental: ${tema}`;
        }
    }

    function abrirPopup() {
        popupInfo.style.display = 'flex';
        
        // Resetar dados coletados se voltando para etapa 2
        if (etapaAtual === 2) {
            nosColetados = [];
        }
        
        atualizarPopup();
    }

    function fecharPopup() {
        popupInfo.style.display = 'none';
    }

    function atualizarPopup() {
        // Atualizar título do popup
        const titulos = {
            1: 'Tema Central',
            2: 'Nós do Mapa Mental',
            3: 'Finalizar'
        };
        document.getElementById('popup-titulo').textContent = titulos[etapaAtual];

        // Mostrar/ocultar etapas do formulário
        etapasForm.forEach(etapa => {
            etapa.classList.remove('ativa');
        });
        document.getElementById(`etapa-${etapaAtual}`).classList.add('ativa');

        // Atualizar botões
        btnAnterior.style.display = etapaAtual > 1 ? 'block' : 'none';
        
        if (etapaAtual === 1) {
            btnProximo.textContent = 'Próximo';
        } else if (etapaAtual === 2) {
            btnProximo.textContent = nosColetados.length > 0 ? 'Próximo Nó' : 'Adicionar Nó';
        } else {
            btnProximo.textContent = 'Gerar Mapa Mental';
        }

        // Atualizar informações na etapa 2
        if (etapaAtual === 2) {
            const infoNos = document.getElementById('info-nos');
            if (nosColetados.length > 0) {
                const listaNos = nosColetados.map(no => 
                    `• ${no.subtema} (${no.detalhes.length} detalhes, ${no.conexoes.length} conexões)`
                ).join('<br>');
                infoNos.innerHTML = `<strong>Nós adicionados (${nosColetados.length}):</strong><br>${listaNos}`;
            } else {
                infoNos.innerHTML = '<em>Nenhum nó adicionado ainda</em>';
            }
        }

        // Atualizar etapas na tela de opções
        etapasOpcoes.forEach(etapa => {
            const numEtapa = parseInt(etapa.dataset.etapa);
            etapa.classList.remove('ativa');
            if (numEtapa === etapaAtual) {
                etapa.classList.add('ativa');
            }
            if (numEtapa <= etapaAtual) {
                etapa.classList.remove('bloqueada');
            } else {
                etapa.classList.add('bloqueada');
            }
        });
    }

    // Fechar popup ao clicar fora
    popupInfo.addEventListener('click', function (e) {
        if (e.target === popupInfo) {
            fecharPopup();
        }
    });
});
