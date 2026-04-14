// Graph.hpp
// SPDX-License-Identifier: BSD-2-Clause
#pragma once

#include <cassert>
#include <cinttypes>
#include <list>
#include <optional>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace App {

class Graph {
public:
    using Vtx = uint32_t;
    using Weight = uint32_t;
    static constexpr Weight k_InfWeight = 0xFFFFFFFF;

    struct Adj {
        Weight weight;
        Vtx v;
        Adj() = default;
        Adj(Vtx v_, Weight w_) : weight(w_), v(v_) {}
    };

    struct Edge {
        Weight weight;
        Vtx u, v;
        Edge() = default;
        Edge(Weight w_, Vtx u_, Vtx v_) : weight(w_), u(u_), v(v_) {}
    };

    uint32_t n() const {
        assert(m_Adj.size() <= 0xFFFFFFFF);
        return static_cast<uint32_t>(m_Adj.size());
    }

    void setN(uint32_t n) {
        // Passo 1: Redimensionar o vetor primário
        m_Adj.resize(n);
        if (m_Adj.size() < m_Adj.capacity() >> 1) {
            // Limpar memória desnecessária
            m_Adj.shrink_to_fit();
        }
        // Passo 2: Limpar vértices excedendo n - 1
        for (auto& list : m_Adj)
            list.remove_if([n](const Adj& a) { return a.v >= n; });
    }

    void insert(const Edge& e) { m_Adj.at(e.u).emplace_back(e.v, e.weight); }

    void remove(const Edge& e) {
        m_Adj.at(e.u).remove_if([&e](const Adj& a) { return a.v == e.v; });
    }

    template <class Container = std::vector<Adj>>
    Container adj(Vtx u) const {
        return Container(m_Adj.at(u).cbegin(), m_Adj.at(u).cend());
    }

    template <class Container = std::vector<Edge>>
    Container adjEdges(Vtx u) const {
        Container rv;
        for (const auto& a : m_Adj.at(u))
            rv.emplace_back(a.weight, u, a.v);
        return rv;
    }

    template <class Container = std::vector<Edge>>
    Container edges() const {
        Container rv;
        for (Vtx i = 0; i < static_cast<Vtx>(m_Adj.size()); i++)
            for (const auto& a : m_Adj[i])
                rv.emplace_back(a.weight, i, a.v);
        return rv;
    }

    template <class Container = std::vector<Weight>>
    void dijkstra(Vtx source, Container* pDist, Graph* pSpt) const {
        /**
         * Implementação baseada em:
         * https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Using_a_priority_queue (seção do
         * pseudocódigo)
         */

        if (n() != 0 && source >= n())
            throw std::out_of_range("Graph::dijkstra(): source fora do intervalo");
        if (!pDist && !pSpt)
            throw std::logic_error("Graph::dijkstra(): pDist e pSpt nao podem ser ambos nulos");

        // Os arrays auxiliares serão indexados por vértice por padrão
        std::vector<Weight> dist(n(), k_InfWeight);
        std::vector<int64_t> prev(n(), -1);  // -1 = vértice indefinido
        // prev[source] = 0;
        dist[source] = 0;

        // (Na STL, o predicado do operador > faz uma min-heap, e vice-versa)
        auto cmp = [](const Adj& a, const Adj& b) { return a.weight > b.weight; };
        // Dessa vez não vai dar para usar simplesmente std::priority_queue porque precisamos ser
        // capazes de mudar a prioridade de elementos que possam não ser o topo
        std::priority_queue<Adj, std::vector<Adj>, decltype(cmp)> q(cmp);
        q.push(Adj(source, 0));

        while (!q.empty()) {
            Adj top = q.top();
            q.pop();
            Vtx u = top.v;
            Weight du = top.weight;
            if (du > dist[u])
                continue;  // entrada obsoleta

            for (const Adj& arc : adj(u)) {
                Weight alt = dist[u] + arc.weight;
                if (alt < dist[arc.v]) {
                    prev[arc.v] = static_cast<int64_t>(u);
                    dist[arc.v] = alt;
                    q.push(Adj(arc.v, alt));
                }
            }
        }

        if (pDist) {
            if constexpr (std::is_same<decltype(dist), Container>::value)
                *pDist = std::move(dist);
            else
                *pDist = Container(dist.cbegin(), dist.cend());
        }

        if (pSpt) {
            *pSpt = Graph{};
            pSpt->setN(n());
            for (Vtx v = 0; v < n(); v++) {
                if (prev[v] >= 0 && v != source) {
                    Vtx p = static_cast<Vtx>(prev[v]);
                    Weight w = dist[v] - dist[p];
                    pSpt->insert(Edge(w, p, v));
                }
            }
        }
    }

    Graph prim(Vtx start = 0) const {
        // Limitar start ao intervalo de vértices presente/existente no grafo
        if (n() != 0 && start >= n())
            throw std::out_of_range("Graph::prim(): start fora do intervalo");

        /**
         * Implementação baseada em:
         * https://en.wikipedia.org/wiki/Prim%27s_algorithm#Description (seção do pseudocódigo)
         */

        // Os arrays auxiliares serão indexados por vértice por padrão
        std::vector<Weight> cheapestCost(n(), k_InfWeight);
        std::vector<std::optional<Edge>> cheapestEdge(n(), std::nullopt);
        std::vector<bool> explored(n(), false);

        // (Na STL, o predicado do operador > faz uma min-heap, e vice-versa)
        auto cmp = [](const Adj& a, const Adj& b) { return a.weight > b.weight; };
        // unexplored é consultado sempre pelo menor custo, então será uma min-heap
        std::priority_queue<Adj, std::vector<Adj>, decltype(cmp)> unexplored(cmp);

        cheapestCost[start] = 0;
        unexplored.push(Adj(start, 0));

        while (!unexplored.empty()) {
            // Selecionar vértice não-explorado com menor custo
            Vtx cur = unexplored.top().v;
            unexplored.pop();
            if (explored[cur])
                continue;
            explored[cur] = true;

            for (const Adj& a : adj(cur)) {
                if (!explored[a.v] && a.weight < cheapestCost[a.v]) {
                    cheapestCost[a.v] = a.weight;
                    cheapestEdge[a.v] = Edge(a.weight, cur, a.v);
                    unexplored.push(Adj(a.v, a.weight));
                }
            }
        }

        Graph result;
        result.setN(n());
        for (Vtx v = 0; v < n(); v++)
            if (cheapestEdge[v])
                result.insert(*cheapestEdge[v]);
        return result;
    }

private:
    std::vector<std::list<Adj>> m_Adj;
};

}  // namespace App
