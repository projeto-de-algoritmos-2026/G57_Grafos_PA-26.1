// AppData.hpp
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include <string_view>
#include "Graph.hpp"

#define APP_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

namespace App {

// ---------------------------------------------------------------------------
// Lugares do campus FCTE — índice = vértice no grafo
// ---------------------------------------------------------------------------
static constexpr std::string_view g_Lugares[] = {
    "Obelisco",                // 0
    "Auditório",               // 1
    "Corredor, frente à I1",   // 2
    "Corredor, frente à I5",   // 3
    "Corredor, frente à I10",  // 4
    "Laboratórios",            // 5
    "RU",                      // 6
    "Quadra",                  // 7
    "Contêineres",             // 8
    "Saída Principal",         // 9
    "I1",                      // 10
    "I5",                      // 11
    "I10",                     // 12
};

static constexpr uint32_t k_NumLugares = APP_ARRAY_SIZE(g_Lugares);

// ---------------------------------------------------------------------------
// Arestas do grafo (não-direcionado — cada par é inserido nos dois sentidos)
// (Essencialmente, uma versão de Graph::Edge fora da classe, mesmo...)
// Pesos representam distância aproximada em metros entre os lugares
// ---------------------------------------------------------------------------
struct EdgeDef {
    Graph::Vtx u, v;
    Graph::Weight weight;
};

static constexpr EdgeDef g_Arestas[] = {
    {9, 3, 260},  // Saída Principal <-> Corredor, frente à I5
    {9, 2, 250},  // Saída Principal <-> Corredor, frente à I1
    {2, 3, 25},   // Corredor, frente à I1 <-> Corredor, frente à I5
    {3, 4, 25},   // Corredor, frente à I5 <-> Corredor, frente à I10
    {2, 0, 35},   // Corredor, frente à I1 <-> Obelisco
    {0, 1, 20},   // Obelisco <-> Auditório
    {0, 4, 60},   // Obelisco <-> Corredor, frente à I10
    {0, 6, 110},  // Obelisco <-> RU
    {1, 5, 80},   // Auditório <-> Laboratórios
    {4, 6, 130},  // Corredor, frente à I10 <-> RU
    {6, 7, 40},   // RU <-> Quadra
    {7, 8, 65},   // Quadra <-> Contêineres
    {5, 8, 120},  // Laboratórios <-> Contêineres
    {0, 5, 90},   // Obelisco <-> Laboratórios
    {3, 0, 50},   // Corredor, frente à I5 <-> Obelisco
    {2, 10, 10},  // Corredor, frente à I1 <-> I1
    {3, 11, 10},  // Corredor, frente à I5 <-> I5
    {4, 12, 10},  // Corredor, frente à I10 <-> I10
};

// ---------------------------------------------------------------------------
// Constrói e retorna o grafo do campus já preenchido
// ---------------------------------------------------------------------------
inline Graph buildCampusGraph() {
    Graph g;
    g.setN(k_NumLugares);
    for (const auto& e : g_Arestas) {
        // Embora a classe Graph tenha sido projetada como grafo direcionado por padrão,
        // no final todos os caminhos decididos acabaram sendo, mesmo, não-direcionados,
        // então o reproduzimos aqui inserindo uma duplicata direcionada "invertida" para
        // cada aresta originalmente não-direcionada que seria direcionada:
        g.insert({e.weight, e.u, e.v});
        g.insert({e.weight, e.v, e.u});
    }
    return g;
}

}  // namespace App
