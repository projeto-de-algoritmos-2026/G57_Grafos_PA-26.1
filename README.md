# G57_Grafos_PA-26.1

Número da Lista: 57<br>
Conteúdo da Disciplina: Grafos<br>

## Alunos
|Matrícula | Aluno |
| -- | -- |
| 23/1011927  |  **João Felipe** Oliveira Alves e Silva |
| 22/2006356  |  **Pedro** Lock Martins |

## Sobre 
Este é um projeto relativamente simples, porém ainda bem funcional, em C++ usando **grafos** para implementar como seria uma aplicação de "mapas" para encontrar os melhores (mais curtos) caminhos entre 2 pontos no *campus* da UnB/FCTE, além de uma funcionalidade adicional de fornecer ao usuário o melhor caminho para se percorrer todos os demais pontos de dito *campus* a partir de um determinado ponto de origem.<br>
Para essas funções, foram trabalhados, respectivamente, os algoritmos de **Dijkstra** – busca de caminho de **menor custo em grafos ponderados** – e de **Prim** – uma forma simples, porém ainda eficiente, de se produzir uma **Árvore Geradora Mínima (*Minimum Spanning Tree* – MST)**, também **em grafos ponderados** –.

## Screenshots
### Captura de Tela 1: Menu principal
![Captura de Tela 1: Menu principal](<docs/assets/Captura de Tela 1.png>)
### Captura de Tela 2: Uso da função "Caminho mais curto (Dijkstra)"
![Captura de Tela 2: Uso da função "Caminho mais curto (Dijkstra)](<docs/assets/Captura de Tela 2.png>)
### Captura de Tela 3: Uso da função "*Tour* do *campus* (MST — Prim)"
![Captura de Tela 3: Uso da função "Tour do campus (MST — Prim)"](<docs/assets/Captura de Tela 3.png>)
### Captura de Tela 4: Uso da função "Listar lugares"
![Captura de Tela 4: Uso da função "Listar lugares"](<docs/assets/Captura de Tela 4.png>)

## Instalação 
Linguagem: C++<br>
Framework: *Nenhum*<br>
<br>
O projeto depende basicamente apenas da C++ STL (*Standard Template Library*), e por extensão da biblioteca padrão C.
No Windows, também precisa dos cabeçalhos da *Windows API* para a preparação do *console*, que podem ser obtidos com
a instalação do Microsoft Visual Studio, por exemplo.
De qualquer forma, o compilador C++ precisa estar suportando a versão C++17 (em diante) da linguagem.<br>
<br>
Para compilar, rode o CMake com o diretório de *Source* sendo a própria pasta deste projeto, e o diretório de *Build* da sua escolha,
mas é preferencialmente recomendado o `{raiz_do_diretório_do_projeto}/build`. Opcionalmente, pode especificar também, por exemplo, um *Generator*
(formato dos arquivos de compilação a serem gerados, ex. Makefile, Ninja, solução do MS Visual Studio etc.) da sua escolha além do padrão.<br>
Por exemplo, usando a linha de comando:<br>
`$ cmake -S . -B ./build` (Gerador padrão do sistema), ou:<br>
`$ cmake -S . -B ./build -G "Ninja Multi-Config"` (Especificando o gerador *Ninja Multi-Config*).<br><br>
Após isso, rode normalmente o arquivo de compilação correspondente na pasta de *Build* selecionada para compilar o programa em si, e então, se houver sucesso até aqui, se poderá executá-lo. Por padrão, o executável gerado deverá ter o nome de `main`, ou *mesmo* `main.exe` no Windows.

## Uso 
Ao abrir o executável compilado, será exibido um menu no *console* conforme a **Captura de Tela 1**. Digite o número de uma ação a se executar, e pressione a tecla Enter.<br>
Dependendo da opção válida selecionada, o programa irá te solicitar que digite também o número de cada parâmetro que deseje passar à ação, conforme a parte inicial das **Capturas de Tela 2 ou 3**.<br>
Quando estiver pronto o resultado da operação, ele será mostrado no *console* conforme a parte final das **Capturas de Tela 2 ou 3**. Então, pressione Enter para voltar ao menu principal.

## Outros 
* A implementação específica de grafo usada na aplicação é a lista de adjacências, considerada no geral o melhor "custo-benefício" para diversas aplicações gerais/genéricas.
* O projeto usa uma só unidade de compilação (`.cpp`) e implementa funcionalidades auxiliares simplesmente em cabeçalhos se necessário, o que não só deixa a compilação mais trivial – a ponto de potencialmente nem ser tão necessário para conveniência quanto (boa) apresentação o projeto CMake aqui –, como a torna mais rápida do que com múltiplas UCs – embora, admitidamente, neste caso não deva ser tão notável por ser um projeto de código tão (relativamente?) pequeno –;
* Chegamos a saber de uma sugestão de adicionar nós do grafo para o entrepiso de cada escada, para poder se especificar granularmente também o custo (a distância) de se subir e/ou descer uma escada. Porém, um de nós apontou, em contrapartida, que na elaboração do conjunto de dados para o aplicativo esses "custos" já foram considerados e embutidos como parte de arestas/caminhos pré-existentes.