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
    const totalEtapas = 5;

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
        if (etapaAtual < totalEtapas) {
            etapaAtual++;
            atualizarPopup();
        } else {
            // Última etapa - submeter formulário
            document.getElementById('form-info').submit();
        }
    });

    function abrirPopup() {
        popupInfo.style.display = 'flex';
        atualizarPopup();
    }

    function fecharPopup() {
        popupInfo.style.display = 'none';
    }

    function atualizarPopup() {
        // Atualizar título do popup
        const titulos = {
            1: 'Tema Central',
            2: 'Subtemas',
            3: 'Detalhes',
            4: 'Recursos Visuais',
            5: 'Finalizar'
        };
        document.getElementById('popup-titulo').textContent = titulos[etapaAtual];

        // Mostrar/ocultar etapas do formulário
        etapasForm.forEach(etapa => {
            etapa.classList.remove('ativa');
        });
        document.getElementById(`etapa-${etapaAtual}`).classList.add('ativa');

        // Atualizar botões
        btnAnterior.style.display = etapaAtual > 1 ? 'block' : 'none';
        btnProximo.textContent = etapaAtual === totalEtapas ? 'Gerar Mapa Mental' : 'Próximo';

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
