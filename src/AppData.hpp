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
static const std::string_view g_Lugares[] = {
    "Obelisco",          // 0
    "Auditório",         // 1
    "I1",                // 2
    "I5",                // 3
    "I10",               // 4
    "Laboratórios",      // 5
    "RU",                // 6
    "Quadra",            // 7
    "Containers",        // 8
    "Saída Principal",   // 9
};

static constexpr uint32_t k_NumLugares = APP_ARRAY_SIZE(g_Lugares);

// ---------------------------------------------------------------------------
// Arestas do grafo (não-direcionado — cada par é inserido nos dois sentidos)
// Pesos representam distância aproximada em metros entre os lugares
// ---------------------------------------------------------------------------
struct EdgeDef {
    Graph::Vtx u, v;
    Graph::Weight weight;
};

static const EdgeDef g_Arestas[] = {
    { 9,  3,  260},  // Saída Principal <-> I5
    { 9,  2,  250},  // Saída Principal <-> I1
    { 2,  3,   25},  // I1 <-> I5
    { 3,  4,   25},  // I5 <-> I10
    { 2,  0,   35},  // I1 <-> Obelisco
    { 0,  1,   20},  // Obelisco <-> Auditório
    { 0,  4,   60},  // Obelisco <-> I10
    { 0,  6,  110},  // Obelisco <-> RU
    { 1,  5,   80},  // Auditório <-> Laboratórios
    { 4,  6,  130},  // I10 <-> RU
    { 6,  7,   40},  // RU <-> Quadra
    { 7,  8,   65},  // Quadra <-> Containers
    { 5,  8,  120},  // Laboratórios <-> Containers
    { 0,  5,   90},  // Obelisco <-> Laboratórios
    { 3,  0,   50},  // I5 <-> Obelisco
};

// ---------------------------------------------------------------------------
// Constrói e retorna o grafo do campus já preenchido
// ---------------------------------------------------------------------------
inline Graph buildCampusGraph() {
    Graph g;
    g.setN(k_NumLugares);
    for (const auto& e : g_Arestas) {
        g.insert({e.weight, e.u, e.v});
        g.insert({e.weight, e.v, e.u});
    }
    return g;
}

}  // namespace App
