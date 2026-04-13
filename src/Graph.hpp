// Graph.hpp
// SPDX-License-Identifier: BSD-2-Clause

#include <cassert>
#include <cinttypes>
#include <list>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
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
    };

    struct Edge {
        Weight weight;
        Vtx u, v;

#if 0
        constexpr bool operator<(const Edge& rhs) const {
            // A comparação (mais) "importante"
            if (weight != rhs.weight)
                return weight < rhs.weight;
            // Provavelmente desnecessário para nossos fins mas não deverá
            // afetar negativamente os resultados operacionais, então...:
            if (u != rhs.u)
                return u < rhs.u;
            return v < rhs.v;
        }
#endif  // 0
    };

    uint32_t n() const {
        assert(m_Adj.size() <= 0xFFFFFFFF);
        return m_Adj.size();
    }

    void setN(uint32_t n) {
        uint32_t oldN = this->n();
        // Passo 1: Redimensionar o vetor primário
        m_Adj.resize(n);
        if (m_Adj.size() < m_Adj.capacity() >> 1) {
            // Limpar memória desnecessária
            m_Adj.shrink_to_fit();
        }
        // Passo 2: Limpar vértices excedendo n - 1
        for (auto& list : m_Adj) {
            list.remove_if([n](const auto& adj) { return adj.v >= n; });
        }
    }

    void insert(const Edge& e) {
        const auto& [w, u, v] = e;
        m_Adj.at(u).emplace_back(v, w);
    }

    void remove(const Edge& e) {
        const auto& [u, v, _] = e;
        m_Adj.at(u).remove_if([v](const auto& adj) { return adj.v == v; });
    }

    template <class Container = std::vector<Adj>>
    Container adj(Vtx u) const {
        return Container(m_Adj.at(u).cbegin(), m_Adj.at(u).cend());
    }

    template <class Container = std::vector<Edge>>
    Container adjEdges(Vtx u) const {
        Container rv;
        for (const auto& a : m_Adj.at(u)) {
            rv.emplace_back(a.weight, u, a.v);
        }
        return rv;
    }

    template <class Container = std::vector<Edge>>
    Container edges() const {
        Container rv;
        for (Vtx i = 0; i < m_Adj.size(); i++) {
            for (const auto& a : m_Adj[i]) {
                rv.emplace_back(a.weight, i, a.v);
            }
        }
        return rv;
    }

    template <class Container = std::vector<Weight>>
    void dijkstra(Vtx source, Container* pDist, Graph* pSpt) const {
        /**
         * Implementação baseada em:
         * https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Using_a_priority_queue (seção do
         * pseudocódigo)
         */

        if (n() != 0 && source >= n()) {
            throw std::out_of_range(
                "Graph::dijkstra(): Argumento {source} deve ser menor que Graph::n()");
        }

        if (!pDist && !pSpt) {
            throw std::logic_error("Graph::dijkstra(): pelo menos um dos argumentos {pSpt} e "
                                   "{pDist} não deve ser nulo");
        }

        // Os arrays auxiliares serão indexados por vértice por padrão
        std::vector<Weight> dist(n(), k_InfWeight);
        std::vector<int64_t> prev(n(), -1);  // -1 = vértice indefinido
        // prev[source] = 0;
        dist[source] = 0;

        // (Na STL, o predicado do operador > faz uma min-heap, e vice-versa)
        static const auto g_HeapPredicate = [](Adj u, Adj v) { return u.weight > v.weight; };
        // Dessa vez não vai dar para usar simplesmente std::priority_queue porque precisamos ser
        // capazes de mudar a prioridade de elementos que possam não ser o topo
        std::priority_queue<Adj, std::vector<Adj>, decltype(g_HeapPredicate)> q(g_HeapPredicate);

        q.emplace(0, source);

        while (!q.empty()) {
            auto [du, u] = q.top();
            q.pop();
            if (du > dist[u])
                continue;  // entrada obsoleta

            for (const auto& arc : adj(u)) {
                Weight alt = dist[u] + arc.weight;
                if (alt < dist[arc.v]) {
                    prev[arc.v] = u;
                    dist[arc.v] = alt;
                    q.emplace(alt, arc.v);
                }
            }
        }

        if (pDist) {
            if constexpr (std::is_same<decltype(dist), Container>::value) {
                *pDist = std::move(dist);
            } else {
                *pDist = Container(dist.cbegin(), dist.cend());
            }
        }

        if (pSpt) {
            *pSpt = Graph{};
            for (Vtx v = 0; v < n(); v++) {
                if (prev[v] >= 0 && v != source) {
                    Weight w = dist[v] - dist[prev[v]];
                    pSpt->insert(Edge{w, prev[v], v});
                }
            }
        }
    }

    Graph prim(Vtx start = 0) const {
        // Limitar start ao intervalo de vértices presente/existente no grafo
        if (n() != 0 && start >= n()) {
            throw std::out_of_range(
                "Graph::prim(): Argumento {start} deve ser menor que Graph::n()");
        }

        /**
         * Implementação baseada em:
         * https://en.wikipedia.org/wiki/Prim%27s_algorithm#Description (seção do pseudocódigo)
         */

        // Os arrays auxiliares serão indexados por vértice por padrão
        std::vector<Weight> cheapestCost(n(), k_InfWeight);
        std::vector<std::optional<Edge>> cheapestEdge(n(), std::nullopt);
        std::vector<bool> explored(n(), false);

        // (Na STL, o predicado do operador > faz uma min-heap, e vice-versa)
        static const auto g_HeapPredicate = [](Adj u, Adj v) { return u.weight > v.weight; };
        // unexplored é consultado sempre pelo menor custo, então será uma min-heap
        std::priority_queue<Adj, std::vector<Adj>, decltype(g_HeapPredicate)> unexplored(
            g_HeapPredicate);

        cheapestCost[start] = 0;
        unexplored.emplace(0, start);

        while (!unexplored.empty()) {
            // Selecionar vértice não-explorado com menor custo
            auto [cost, currentVtx] = unexplored.top();
            unexplored.pop();
            if (explored[currentVtx])
                continue;
            explored[currentVtx] = true;

            for (const Adj& adj : adj(currentVtx)) {
                const Vtx& neighbor = adj.v;
                if (!explored[neighbor] && adj.weight < cheapestCost[neighbor]) {
                    cheapestCost[neighbor] = adj.weight;
                    cheapestEdge[neighbor] = Edge{adj.weight, currentVtx, adj.v};
                    unexplored.emplace(adj.weight, neighbor);
                }
            }
        }

        Graph resultEdges;
        for (Vtx vertex = 0; vertex < n(); vertex++) {
            const auto& cheapest = cheapestEdge[vertex];
            if (cheapest != std::nullopt) {
                resultEdges.insert(*cheapest);
            }
        }

        return resultEdges;
    }

private:
    std::vector<std::list<Adj>> m_Adj;
};
}  // namespace App