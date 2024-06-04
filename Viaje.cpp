#include <iostream>
#include <string>
#include <queue>
#include <algorithm>

struct Viaje {
    std::string matricula;
    std::string nombreEmbarcacion;
    int dia, mes, anio;
    double precio;
    std::string destino;
    int capacidad;
    std::queue<std::string> pasajeros;

    std::string generarIdentificador() const {
        return matricula.substr(0, 2) + std::to_string(anio) +
               (mes < 10 ? "0" : "") + std::to_string(mes) +
               (dia < 10 ? "0" : "") + std::to_string(dia);
    }
};

struct NodoAVL {
    Viaje viaje;
    NodoAVL *izq, *der;
    int altura;

    NodoAVL(const Viaje &v) : viaje(v), izq(nullptr), der(nullptr), altura(1) {}
};

class ArbolAVL {
private:
    NodoAVL *raiz;

    int altura(NodoAVL *nodo) {
        return nodo ? nodo->altura : 0;
    }

    int obtenerBalance(NodoAVL *nodo) {
        return nodo ? altura(nodo->izq) - altura(nodo->der) : 0;
    }

    NodoAVL* rotarDerecha(NodoAVL *y) {
        NodoAVL *x = y->izq;
        NodoAVL *T2 = x->der;
        x->der = y;
        y->izq = T2;
        y->altura = std::max(altura(y->izq), altura(y->der)) + 1;
        x->altura = std::max(altura(x->izq), altura(x->der)) + 1;
        return x;
    }

    NodoAVL* rotarIzquierda(NodoAVL *x) {
        NodoAVL *y = x->der;
        NodoAVL *T2 = y->izq;
        y->izq = x;
        x->der = T2;
        x->altura = std::max(altura(x->izq), altura(x->der)) + 1;
        y->altura = std::max(altura(y->izq), altura(y->der)) + 1;
        return y;
    }

    NodoAVL* insertar(NodoAVL *nodo, const Viaje &viaje) {
        if (!nodo) return new NodoAVL(viaje);

        if (viaje.generarIdentificador() < nodo->viaje.generarIdentificador()) {
            nodo->izq = insertar(nodo->izq, viaje);
        } else if (viaje.generarIdentificador() > nodo->viaje.generarIdentificador()) {
            nodo->der = insertar(nodo->der, viaje);
        } else {
            return nodo;
        }

        nodo->altura = 1 + std::max(altura(nodo->izq), altura(nodo->der));
        int balance = obtenerBalance(nodo);

        if (balance > 1 && viaje.generarIdentificador() < nodo->izq->viaje.generarIdentificador()) {
            return rotarDerecha(nodo);
        }

        if (balance < -1 && viaje.generarIdentificador() > nodo->der->viaje.generarIdentificador()) {
            return rotarIzquierda(nodo);
        }

        if (balance > 1 && viaje.generarIdentificador() > nodo->izq->viaje.generarIdentificador()) {
            nodo->izq = rotarIzquierda(nodo->izq);
            return rotarDerecha(nodo);
        }

        if (balance < -1 && viaje.generarIdentificador() < nodo->der->viaje.generarIdentificador()) {
            nodo->der = rotarDerecha(nodo->der);
            return rotarIzquierda(nodo);
        }

        return nodo;
    }

    NodoAVL* eliminar(NodoAVL *raiz, const std::string &id) {
        if (!raiz) return raiz;

        if (id < raiz->viaje.generarIdentificador()) {
            raiz->izq = eliminar(raiz->izq, id);
        } else if (id > raiz->viaje.generarIdentificador()) {
            raiz->der = eliminar(raiz->der, id);
        } else {
            if ((!raiz->izq) || (!raiz->der)) {
                NodoAVL *temp = raiz->izq ? raiz->izq : raiz->der;
                if (!temp) {
                    temp = raiz;
                    raiz = nullptr;
                } else {
                    *raiz = *temp;
                }
                delete temp;
            } else {
                NodoAVL *temp = raiz->der;
                while (temp->izq) temp = temp->izq;
                raiz->viaje = temp->viaje;
                raiz->der = eliminar(raiz->der, temp->viaje.generarIdentificador());
            }
        }

        if (!raiz) return raiz;

        raiz->altura = 1 + std::max(altura(raiz->izq), altura(raiz->der));
        int balance = obtenerBalance(raiz);

        if (balance > 1 && obtenerBalance(raiz->izq) >= 0) {
            return rotarDerecha(raiz);
        }

        if (balance > 1 && obtenerBalance(raiz->izq) < 0) {
            raiz->izq = rotarIzquierda(raiz->izq);
            return rotarDerecha(raiz);
        }

        if (balance < -1 && obtenerBalance(raiz->der) <= 0) {
            return rotarIzquierda(raiz);
        }

        if (balance < -1 && obtenerBalance(raiz->der) > 0) {
            raiz->der = rotarDerecha(raiz->der);
            return rotarIzquierda(raiz);
        }

        return raiz;
    }

    NodoAVL* buscar(NodoAVL *nodo, const std::string &id) {
        if (!nodo || nodo->viaje.generarIdentificador() == id) {
            return nodo;
        }
        if (id < nodo->viaje.generarIdentificador()) {
            return buscar(nodo->izq, id);
        }
        return buscar(nodo->der, id);
    }

    void inOrden(NodoAVL *nodo) {
        if (nodo) {
            inOrden(nodo->izq);
            std::cout << "ID: " << nodo->viaje.generarIdentificador() << " - Destino: " << nodo->viaje.destino << std::endl;
            inOrden(nodo->der);
        }
    }

public:
    ArbolAVL() : raiz(nullptr) {}

    void registrarViaje(const Viaje &viaje) {
        raiz = insertar(raiz, viaje);
    }

    void buscarViaje(const std::string &id) {
        NodoAVL *nodo = buscar(raiz, id);
        if (nodo) {
            std::cout << "Viaje encontrado: " << std::endl;
            std::cout << "ID: " << nodo->viaje.generarIdentificador() << " - Destino: " << nodo->viaje.destino << std::endl;
        } else {
            std::cout << "Viaje no encontrado." << std::endl;
        }
    }

    void listarViajes() {
        inOrden(raiz);
    }

    void eliminarViaje(const std::string &id) {
        raiz = eliminar(raiz, id);
    }

    void registrarPasajero(const std::string &id, const std::string &pasajero) {
        NodoAVL *nodo = buscar(raiz, id);
        if (nodo) {
            if (nodo->viaje.pasajeros.size() < nodo->viaje.capacidad) {
                nodo->viaje.pasajeros.push(pasajero);
                std::cout << "Pasajero registrado." << std::endl;
            } else {
                std::cout << "Capacidad de la embarcación alcanzada." << std::endl;
            }
        } else {
            std::cout << "Viaje no encontrado." << std::endl;
        }
    }

    void listarPasajeros(const std::string &id) {
        NodoAVL *nodo = buscar(raiz, id);
        if (nodo) {
            std::queue<std::string> temp = nodo->viaje.pasajeros;
            while (!temp.empty()) {
                std::cout << "Pasajero: " << temp.front() << std::endl;
                temp.pop();
            }
        } else {
            std::cout << "Viaje no encontrado." << std::endl;
        }
    }
};

void mostrarMenu() {
    std::cout << "1. Registrar un nuevo viaje" << std::endl;
    std::cout << "2. Buscar un viaje por identificador" << std::endl;
    std::cout << "3. Listar todos los viajes" << std::endl;
    std::cout << "4. Eliminar un viaje por identificador" << std::endl;
    std::cout << "5. Registrar un pasajero en un viaje" << std::endl;
    std::cout << "6. Listar todos los pasajeros de un viaje" << std::endl;
    std::cout << "7. Salir" << std::endl;
    std::cout << "Elige una opción: ";
}

int main() {
    ArbolAVL arbol;
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;
        std::cin.ignore();

        switch (opcion) {
            case 1: {
                Viaje viaje;
                std::cout << "Matricula: ";
                std::getline(std::cin, viaje.matricula);
                std::cout << "Nombre de la embarcacion: ";
                std::getline(std::cin, viaje.nombreEmbarcacion);
                std::cout << "Dia: ";
                std::cin >> viaje.dia;
                std::cout << "Mes: ";
                std::cin >> viaje.mes;
                std::cout << "Anio: ";
                std::cin >> viaje.anio;
                std::cout << "Precio: ";
                std::cin >> viaje.precio;
                std::cin.ignore();
                std::cout << "Destino: ";
                std::getline(std::cin, viaje.destino);
                std::cout << "Capacidad: ";
                std::cin >> viaje.capacidad;
                std::cin.ignore();

                arbol.registrarViaje(viaje);
                break;
            }
            case 2: {
                std::string id;
                std::cout << "Identificador del viaje: ";
                std::getline(std::cin, id);
                arbol.buscarViaje(id);
                break;
            }
            case 3: {
                arbol.listarViajes();
                break;
            }
            case 4: {
                std::string id;
                std::cout << "Identificador del viaje: ";
                std::getline(std::cin, id);
                arbol.eliminarViaje(id);
                break;
            }
            case 5: {
                std::string id, pasajero;
                std::cout << "Identificador del viaje: ";
                std::getline(std::cin, id);
                std::cout << "Nombre del pasajero: ";
                std::getline(std::cin, pasajero);
                arbol.registrarPasajero(id, pasajero);
                break;
            }
            case 6: {
                std::string id;
                std::cout << "Identificador del viaje: ";
                std::getline(std::cin, id);
                arbol.listarPasajeros(id);
                break;
            }
            case 7:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida." << std::endl;
                break;
        }
    } while (opcion != 7);

    return 0;
}
