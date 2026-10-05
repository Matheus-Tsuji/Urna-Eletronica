# Urna Eletrônica 2026 em C

Este projeto é uma simulação do funcionamento da Urna Eletrônica Brasileira, desenvolvida em C puro utilizando a API Win32 para a interface gráfica. O objetivo principal é recriar o fluxo de votação projetado para eleições, oferecendo uma interface fiel ao modelo original e uma lógica de apuração simples e eficiente.

## O que foi implementado

- **Interface Gráfica (GUI):** Criada 100% nativa em C utilizando Win32 API, recriando o layout tradicional da urna com o teclado numérico, botões Branco, Corrige e Confirma, além dos visores de informação (cargo, número digitado, nome do candidato, partido, etc).
- **Leitura de Candidatos:** Os candidatos são carregados a partir do arquivo `candidatos.txt`, permitindo fácil adição ou remoção de nomes sem precisar recompilar o código. O formato utilizado é `Numero;Nome;Partido`.
- **Sistema de Votação:**
  - Validação de votos (dois dígitos para presidente, conforme o modelo).
  - Votos nulos para números inexistentes.
  - Voto em branco.
  - Botão de correção (reinicia a digitação).
  - Feedback de confirmação.
- **Apuração de Votos:** Quando um voto é confirmado, ele é automaticamente contabilizado e salvo no arquivo `apuracao_votos.txt`, que registra a contagem individual de votos válidos, brancos e nulos.

## Como utilizar

1. Execute o arquivo `urna.exe`.
2. Para votar em um candidato, digite os dois números correspondentes através da interface (os candidatos cadastrados no `candidatos.txt` já estarão disponíveis, como 13, 22, 14 e 55).
3. Caso erre, aperte **CORRIGE** para limpar.
4. Caso queira votar em branco, aperte **BRANCO** e em seguida **CONFIRMA**.
5. Ao confirmar um voto válido ou nulo (número não cadastrado), aperte **CONFIRMA**.
6. O arquivo `apuracao_votos.txt` será atualizado com os votos em tempo real.

## Estrutura dos Arquivos

- `main.c`: Código-fonte principal com a lógica da Urna e interface gráfica Win32.
- `urna.exe`: Arquivo executável já compilado (para Windows).
- `candidatos.txt`: Banco de dados simples contendo a lista dos candidatos disponíveis.
- `apuracao_votos.txt`: Arquivo gerado automaticamente contendo a somatória dos votos. Neste repositório, foi deixado um modelo ilustrativo com valores aleatórios de exemplo.

## Compilação

Caso deseje compilar o projeto manualmente (usando o GCC/MinGW), basta executar o comando na raiz do projeto:

```bash
gcc main.c -o urna.exe -mwindows
```
A flag `-mwindows` é importante para não abrir o prompt de comando atrás da interface gráfica.
