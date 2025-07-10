#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <climits>
#include <fstream>
using namespace std;

// Cores
#define BLUE 0
#define GREEN 1
#define RED 2
#define BLACK 3
#define WHITE 4
#define PURPLE 5

const string nomeCores[] = {"BLUE", "GREEN", "RED", "BLACK", "WHITE", "PURPLE"};


string NomeCor(int cor) {
    switch(cor) {
        case BLUE: return "BLUE";
        case GREEN: return "GREEN";
        case RED: return "RED";
        case BLACK: return "BLACK";
        case WHITE: return "WHITE";
        case PURPLE: return "PURPLE";
        default: return "UNKNOWN";
    }
}

// Struct do vértice
template<typename T>
struct Vertice{
    T valor;
    int indiceMatriz;
    bool visitado = false;
    int cor;

    vector<int> adjacentes;

    void Cria(T dado, int indice){
        valor = dado;
        indiceMatriz = indice;
    }

    T Valor(){
        return valor;
    }

    int Indice(){
        return indiceMatriz;
    }
};

template<typename T, int ORDEM, bool digrafo>
struct Grafo {
    vector<Vertice<T>> verts;
    vector<Vertice<T>> interconectados;
    int etapas = 0;
    vector<int> coresUtilizadas;
    int numNos;

    // Construtor (opcional)
    void Cria() {
        numNos = 0;
        
    }



    bool AcrescentaAresta(int indice1, int indice2) {
        // Verifica se os índices são válidos
        if (indice1 < 0 || indice1 >= verts.size() || 
            indice2 < 0 || indice2 >= verts.size()) {
            return false;
        }

        // Adiciona a aresta nos dois vértices (para grafo não direcionado)
        verts[indice1].adjacentes.push_back(indice2);
        
        // Se não for dígrafo, adiciona a aresta no sentido contrário
        if (!digrafo) {
            verts[indice2].adjacentes.push_back(indice1);
        }
        
        return true;
    }


    bool AcrescentaVertice(T valor){
        if (verts.size()==ORDEM) return false;
        Vertice<T> v;
        v.Cria(valor,verts.size());
        verts.push_back(v);
        return true;
    }

    void BuscaL(int indice, int corAlvo) {
        // Verifica se o índice é válido
        if (indice < 0 || indice >= verts.size()) return;
        
        // Se já está na cor alvo, não faz nada
        if (verts[indice].cor == corAlvo) return;

        int corOriginal = verts[indice].cor;
        queue<int> fila;
        fila.push(indice);
        verts[indice].visitado = true;

        // Limpa a lista de interconectados
        interconectados.clear();
        interconectados.push_back(verts[indice]);

        while (!fila.empty()) {
            int atual = fila.front();
            fila.pop();

            // Muda a cor do vértice atual
            verts[atual].cor = corAlvo;

            // Percorre todos os vizinhos
            for (int vizinho : verts[atual].adjacentes) {
                if (!verts[vizinho].visitado && verts[vizinho].cor == corOriginal) {
                    verts[vizinho].visitado = true;
                    fila.push(vizinho);
                    interconectados.push_back(verts[vizinho]);
                }
            }
        }

        // Reseta os visitados para futuras buscas
        for (auto& v : verts) {
            v.visitado = false;
        }

        // Mostra o resultado
        cout << "\nCores após a inundação:\n";
        for (int i = 0; i < verts.size(); i++) {
            cout << "Vértice " << i << ": " << verts[i].cor << endl;
        }
    }



    // Função para imprimir o grafo
    void Imprimir() {
        for (int i = 0; i < numNos; i++) {
            cout << "Vertice " << i << ": " << NomeCor(verts[i].cor) << endl;
        }
    }
    void ResolverAutomaticamente() {
        int menorPassos = INT_MAX;
        vector<int> melhorSequencia;
        int melhorCor = -1;

        // Testa todas as cores possíveis
        for (int corAlvo = 0; corAlvo <= 5; corAlvo++) {
            auto copia = CloneGrafo();
            vector<int> sequencia;
            int passos = 0;

            while (!EstaUniforme(copia, corAlvo)) {
                int vertice = EncontrarMelhorInicio(copia, corAlvo);
                if (vertice == -1) break;

                AplicarInundacao(copia, vertice, corAlvo);
                sequencia.push_back(vertice);
                passos++;
            }

            if (EstaUniforme(copia, corAlvo) && passos < menorPassos) {
                menorPassos = passos;
                melhorSequencia = sequencia;
                melhorCor = corAlvo;
            }
        }

        // Aplica a melhor solução encontrada
        if (melhorCor != -1) {
            cout << "Melhor cor alvo: " << NomeCor(melhorCor) << endl;
            cout << "Numero de passos: " << melhorSequencia.size() << endl;
            cout << "Sequencia: ";
            for (int v : melhorSequencia) cout << v << " ";
            cout << endl;

            for (int v : melhorSequencia) {
                BuscaL(v, melhorCor);
            }
        } else {
            cout << "Nao foi possivel uniformizar o grafo." << endl;
        }
    }


    vector<Vertice<T>> CloneGrafo() {
        return verts;
    }

    bool EstaUniforme(const vector<Vertice<T>>& grafo, int corAlvo) {
        for (const auto& v : grafo) {
            if (v.cor != corAlvo) return false;
        }
        return true;
    }

    int EncontrarMelhorInicio(const vector<Vertice<T>>& grafo, int corAlvo) {
        int maiorComponente = 0;
        int melhorVertice = -1;
        vector<bool> visitado(ORDEM, false);

        for (int i = 0; i < grafo.size(); i++) {
            if (!visitado[i] && grafo[i].cor != corAlvo) {
                int tamanho = 0;
                queue<int> q;
                q.push(i);
                visitado[i] = true;

                while (!q.empty()) {
                    int atual = q.front();
                    q.pop();
                    tamanho++;

                    for (int vizinho : grafo[atual].adjacentes) {
                        if (!visitado[vizinho] && grafo[vizinho].cor == grafo[atual].cor) {
                            visitado[vizinho] = true;
                            q.push(vizinho);
                        }
                    }
                }

                if (tamanho > maiorComponente) {
                    maiorComponente = tamanho;
                    melhorVertice = i;
                }
            }
        }

        return melhorVertice;
    }

    void AplicarInundacao(vector<Vertice<T>>& grafo, int inicio, int corAlvo) {
        int corOriginal = grafo[inicio].cor;
        queue<int> q;
        q.push(inicio);
        grafo[inicio].cor = corAlvo;

        while (!q.empty()) {
            int atual = q.front();
            q.pop();

            for (int vizinho : grafo[atual].adjacentes) {
                if (grafo[vizinho].cor == corOriginal) {
                    grafo[vizinho].cor = corAlvo;
                    q.push(vizinho);
                }
            }
        }
    }

    bool carregarDeArquivo(const string& nomeArquivo) {
        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) return false;

        int n;
        arquivo >> n;
        verts.resize(n);
        numNos = n;

        // Lê as cores iniciais
        for (int i = 0; i < n; i++) {
            arquivo >> verts[i].cor;
            verts[i].Cria(T(), i); // Assume um valor padrão para T()
        }

        // Lê as arestas
        int x, y;
        while (arquivo >> x >> y) {
            if (x >= 0 && x < n && y >= 0 && y < n) {
                verts[x].adjacentes.push_back(y);
                if (!digrafo) {
                    verts[y].adjacentes.push_back(x);
                }
            }
        }

        arquivo.close();
        return true;
    }

    void mostrarGrafo() {
        cout << "\nEstado atual do grafo:\n";
        for (int i = 0; i < verts.size(); i++) {
            cout << "Vértice " << i << ": " << nomeCores[verts[i].cor];
            cout << " | Vizinhos: ";
            for (int viz : verts[i].adjacentes) {
                cout << viz << " ";
            }
            cout << endl;
        }
    }

    void modoManual() {
        while (!estaUniforme()) {
            mostrarGrafo();

            int vertice, cor;
            cout << "\nDigite o vértice para inundar (0-" << verts.size()-1 << "): ";
            cin >> vertice;
            cout << "Digite a cor desejada (0-5): ";
            cin >> cor;

            if (vertice < 0 || vertice >= verts.size() || cor < 0 || cor > 5) {
                cout << "Entrada inválida! Tente novamente.\n";
                continue;
            }

            BuscaL(vertice, cor);
        }

        cout << "\nGrafo uniformizado em " << etapas << " etapas!\n";
        cout << "Cores utilizadas: ";
        for (int cor : coresUtilizadas) cout << nomeCores[cor] << " ";
        cout << endl;
    }

    bool estaUniforme() {
        if (verts.empty()) return true;
        int primeiraCor = verts[0].cor;
        for (const auto& v : verts) {
            if (v.cor != primeiraCor) return false;
        }
        return true;
    }

    void modoAutomatico() {
        while (!estaUniforme()) {
            mostrarGrafo();

            int corAlvo = encontrarMelhorCor();
            int vertice = encontrarMelhorVertice(corAlvo);

            cout << "\nInundando vértice " << vertice << " com " << nomeCores[corAlvo] << endl;
            BuscaL(vertice, corAlvo);
        }

        cout << "\nGrafo uniformizado em " << etapas << " etapas!\n";
        cout << "Cores utilizadas: ";
        for (int cor : coresUtilizadas) cout << nomeCores[cor] << " ";
        cout << endl;
    }


    int encontrarMelhorCor() {
        unordered_map<int, int> contagem;
        for (const auto& v : verts) {
            contagem[v.cor]++;
        }

        int maiorContagem = 0;
        int melhorCor = verts[0].cor;
        for (const auto& par : contagem) {
            if (par.second > maiorContagem) {
                maiorContagem = par.second;
                melhorCor = par.first;
            }
        }
        return melhorCor;
    }

    int encontrarMelhorVertice(int corAlvo) {
        int maiorComponente = 0;
        int melhorVertice = 0;
        vector<bool> visitado(verts.size(), false);

        for (int i = 0; i < verts.size(); i++) {
            if (!visitado[i] && verts[i].cor != corAlvo) {
                int tamanho = 0;
                queue<int> q;
                q.push(i);
                visitado[i] = true;

                while (!q.empty()) {
                    int atual = q.front();
                    q.pop();
                    tamanho++;

                    for (int vizinho : verts[atual].adjacentes) {
                        if (!visitado[vizinho] && verts[vizinho].cor == verts[atual].cor) {
                            visitado[vizinho] = true;
                            q.push(vizinho);
                        }
                    }
                }

                if (tamanho > maiorComponente) {
                    maiorComponente = tamanho;
                    melhorVertice = i;
                }
            }
        }
        return melhorVertice;
    }
};


// Exemplo de uso
int main() {
    Grafo<string, 100, false> g; // Exemplo com ORDEM=100 e não direcionado
    g.Cria();

    if (!g.carregarDeArquivo("grafo1.txt")) {
        cout << "Erro ao carregar o arquivo grafo.txt" << endl;
        return 1;
    }

    cout << "Escolha o modo:\n1. Manual\n2. Automático\nOpção: ";
    int opcao;
    cin >> opcao;

    if (opcao == 1) {
        g.modoManual();
    } else if (opcao == 2) {
        g.modoAutomatico();
    } else {
        cout << "Opção inválida!" << endl;
    }

    return 0;
}