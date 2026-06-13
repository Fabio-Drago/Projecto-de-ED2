# Sistema de Gestão de Voos

Projecto desenvolvido para a unidade curricular de Estruturas de Dados II.  
Sistema em consola para gestão de aeroportos, voos e rotas, implementado em C e modelado como um grafo dirigido.

## Funcionalidades

- Registo e listagem de aeroportos (código IATA, nome, cidade, país)
- Registo e listagem de voos (código, origem, destino, custo, distância, duração)
- Cálculo de rota de menor custo (Dijkstra)
- Cálculo de rota de menor distância (Dijkstra)
- Cálculo de rota com menor número de escalas (BFS)
- Visualização da rede de voos em modo de texto
- Persistência automática de dados em ficheiro de texto (`dados.txt`)

## Como executar

1. Abrir o projecto no Dev-C++ e adicionar os três ficheiros (`main.c`, `grafo.c`, `grafo.h`)
2. Usar **F11** ou "compilar e executar"

## Tecnologias usadas

- C (sem dependências externas)
- Ficheiro de texto para persistência de dados
