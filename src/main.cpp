// main.cpp
// SPDX-License-Identifier: BSD-2-Clause

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <locale>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif  // NOMINMAX
#include <Windows.h>
#endif  // _WIN32
#include "AppData.hpp"
#include "Graph.hpp"

namespace App {

// ---------------------------------------------------------------------------
// Utilitários de terminal
// ---------------------------------------------------------------------------

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static void pausar() {
    std::cout << "\nPressione Enter para continuar...";
    clearInput();
}

static void imprimirSeparador() {
    std::cout << "----------------------------------------\n";
}

static void imprimirLugares() {
    imprimirSeparador();
    std::cout << "Lugares disponíveis no campus FCTE:\n";
    imprimirSeparador();
    for (uint32_t i = 0; i < k_NumLugares; i++) {
        std::cout << "  [" << i << "] " << g_Lugares[i] << "\n";
    }
    imprimirSeparador();
}

static Graph::Vtx lerVertice(const char* prompt) {
    int32_t v = -1;
    while (true) {
        std::cout << prompt;
        std::cin >> v;
        if (std::cin.fail() || v < 0 || static_cast<uint32_t>(v) >= k_NumLugares) {
            clearInput();
            std::cout << "  Entrada inválida. Digite um número entre 0 e " << k_NumLugares - 1
                      << ".\n";
        } else {
            clearInput();
            return static_cast<Graph::Vtx>(v);
        }
    }
}

// ---------------------------------------------------------------------------
// Funcionalidade 1 — Caminho mais curto (Dijkstra)
// ---------------------------------------------------------------------------

static void menuDijkstra(const Graph& g) {
    std::cout << "\n=== CAMINHO MAIS CURTO (Dijkstra) ===\n";
    imprimirLugares();

    Graph::Vtx origem = lerVertice("Origem:  ");
    Graph::Vtx destino = lerVertice("Destino: ");

    if (origem == destino) {
        std::cout << "\nOrigem e destino são o mesmo lugar.\n";
        pausar();
        return;
    }

    // Rodar Dijkstra guardando apenas distâncias e predecessores
    std::vector<Graph::Weight> dist;
    std::vector<int64_t> prev(k_NumLugares, -1);

    // Dijkstra manual para ter acesso ao prev
    {
        dist.assign(k_NumLugares, Graph::k_InfWeight);
        dist[origem] = 0;
        auto cmp = [](std::pair<Graph::Weight, Graph::Vtx> a,
                      std::pair<Graph::Weight, Graph::Vtx> b) { return a.first > b.first; };
        std::priority_queue<std::pair<Graph::Weight, Graph::Vtx>,
                            std::vector<std::pair<Graph::Weight, Graph::Vtx>>, decltype(cmp)>
            q(cmp);
        q.push({0, origem});
        while (!q.empty()) {
            auto [du, u] = q.top();
            q.pop();
            if (du > dist[u])
                continue;
            for (const auto& arc : g.adj(u)) {
                Graph::Weight alt = dist[u] + arc.weight;
                if (alt < dist[arc.v]) {
                    prev[arc.v] = static_cast<int64_t>(u);
                    dist[arc.v] = alt;
                    q.push({alt, arc.v});
                }
            }
        }
    }

    if (dist[destino] == Graph::k_InfWeight) {
        std::cout << "\nNão existe caminho entre " << g_Lugares[origem] << " e "
                  << g_Lugares[destino] << ".\n";
        pausar();
        return;
    }

    // Reconstruir caminho de trás para frente via prev
    std::vector<Graph::Vtx> caminho;
    {
        Graph::Vtx cur = destino;
        std::vector<bool> visitado(k_NumLugares, false);
        bool ok = true;
        while (cur != origem) {
            if (visitado[cur] || prev[cur] < 0) {
                ok = false;
                break;
            }
            visitado[cur] = true;
            caminho.push_back(cur);
            cur = static_cast<Graph::Vtx>(prev[cur]);
        }
        if (ok) {
            caminho.push_back(origem);
            std::reverse(caminho.begin(), caminho.end());
        } else {
            caminho.clear();
        }
    }

    std::cout << "\nCaminho mais curto:\n";
    imprimirSeparador();
    for (size_t i = 0; i < caminho.size(); i++) {
        if (i > 0)
            std::cout << "  ->  ";
        std::cout << g_Lugares[caminho[i]];
        if (i > 0)
            std::cout << " (" << dist[caminho[i]] << "m acumulado)";
        std::cout << "\n";
    }
    imprimirSeparador();
    std::cout << "Distância total: " << dist[destino] << " metros\n";
    pausar();
}

// ---------------------------------------------------------------------------
// Funcionalidade 2 — Tour do campus (MST via Prim)
// ---------------------------------------------------------------------------

static void menuMST(const Graph& g) {
    std::cout << "\n=== TOUR DO CAMPUS (Árvore Geradora Mínima — Prim) ===\n";
    imprimirLugares();

    Graph::Vtx inicio = lerVertice("Ponto de partida: ");

    Graph mst = g.prim(inicio);

    Graph::Weight custoTotal = 0;
    auto arestas = mst.edges();
    for (const auto& e : arestas)
        custoTotal += e.weight;

    std::cout << "\nConexões da rota mínima para visitar o campus inteiro:\n";
    imprimirSeparador();
    for (const auto& e : arestas) {
        std::cout << "  " << g_Lugares[e.u] << "  ->  " << g_Lugares[e.v] << "  (" << e.weight
                  << "m)\n";
    }
    imprimirSeparador();
    std::cout << "Distância total percorrida: " << custoTotal << " metros\n";
    pausar();
}

// ---------------------------------------------------------------------------
// Menu principal
// ---------------------------------------------------------------------------

static void menuPrincipal() {
    const Graph g = buildCampusGraph();

    while (true) {
        std::cout << "\n╔══════════════════════════════════════╗\n";
        std::cout << "║     Mapa do Campus FCTE — G57        ║\n";
        std::cout << "╠══════════════════════════════════════╣\n";
        std::cout << "║  [1] Caminho mais curto (Dijkstra)   ║\n";
        std::cout << "║  [2] Tour do campus (MST — Prim)     ║\n";
        std::cout << "║  [3] Listar lugares                  ║\n";
        std::cout << "║  [0] Sair                            ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << "Opção: ";

        int32_t opcao = -1;
        std::cin >> opcao;
        clearInput();

        switch (opcao) {
        case 1:
            menuDijkstra(g);
            break;
        case 2:
            menuMST(g);
            break;
        case 3:
            imprimirLugares();
            pausar();
            break;
        case 0:
            std::cout << "Saindo...\n";
            return;
        default:
            std::cout << "Opção inválida.\n";
            break;
        }
    }
}

}  // namespace App

int main() {
// Setar locale para UTF-8
#ifdef _WIN32
    // Setar console do Windows para a página de código UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Habilitar processamento de terminal virtual para saída UTF-8 apropriada
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, mode);
        }
    }
#endif

    std::cin.imbue(std::locale("pt_BR.UTF-8"));
    std::cout.imbue(std::locale("pt_BR.UTF-8"));
    std::cerr.imbue(std::locale("pt_BR.UTF-8"));

    App::menuPrincipal();
    return EXIT_SUCCESS;
}
