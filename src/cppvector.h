/**
 * @file cppvector.h
 * @brief Implementacion de un vector dinamico similar a std::vector
 *
 * Esta clase implementa un contenedor dinamico que almacena elementos tipo 'tipodato'
 * Además, es compatible con algoritmos de STL y es interoperable con std::vector.
 * Proporciona los metodos de std::vector, además de los mismos en Español.
 *
 * @include iostream
 * @include algorithm
 * @include cstring
 * @include limits
 * @include stdexcept
 *
 * @author Marian
 * @date May 31st, 2025
 *
 **/

#ifndef CPPVECTOR_H
#define CPPVECTOR_H

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <ranges>
/**
* @brief Compara la igualdad entre dos valores.
*
* Esta implementación cuenta con una especialización para datos de punto flotante
* y datos que no son de punto flotante.
*
* @tparam T Tipo de datos a comparar.
* @param a Primer valor.
* @param b Segundo valor.
* @return `true` si los valores son iguales, `false` en caso contrario.
*
* @note
* - Para datos que **no son de punto flotante**, utiliza `==` para la comparación.
* - Para datos **de punto flotante**, utiliza una comparación con tolerancia basada en
*   `std::numeric_limits<T>::epsilon()` para evitar errores de precisión.
*
*/

template<typename T>
std::enable_if_t<!std::is_floating_point_v<T>, bool>
areEqual(const T& a, const T& b) {
    return a == b;
}

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, bool>
areEqual(const T& a, const T& b) {
    return std::abs(a - b) <= std::numeric_limits<T>::epsilon();
}

// Inicio vector dinamico

/**
* @struct Vector
* @brief Una implementacion de un vector dinamico similar a std::vector que soporta algoritmos STL.
*
* La estructura Vector soporta la asignacion y manejo de un array de elementos de tipo 'tipodato'.
* Proporciona diferentes metodos de inicializacion, iteradores para el recorrido del vector y soporte
* para move semantics.
*
* @tparam tipodato Tipo de dato almacenado
*/
template<typename tipodato, typename Allocator = std::allocator<tipodato> >
struct Vector {

    template<typename Alloc, typename Ptr, typename... Args>
    void alloc_construct(Alloc& alloc, Ptr ptr, Args&&... args) {
        std::allocator_traits<Alloc>::construct(alloc, ptr, std::forward<Args>(args)...);
    }

    template<typename Alloc, typename Ptr>
    void alloc_destroy(Alloc& alloc, Ptr ptr) {
        std::allocator_traits<Alloc>::destroy(alloc, ptr);
    }

private:
    tipodato *datos_;    /// < Puntero al arreglo dinamico
    size_t tamano_;         /// < Cantidad actual de elementos
    size_t capacidad_;   /// < Capacidad total reservada
    Allocator alloc;    /// < Allocator

    bool ordenado_;     /// < Bool para evitar doble ordenamiento

public:
    using value_type = tipodato;
    using reference = tipodato&;
    using const_reference = const tipodato&;
    using pointer = tipodato*;
    using const_pointer = const tipodato*;
    using iterator = tipodato*;
    using const_iterator = const tipodato*;

    Vector() : datos_(nullptr), tamano_(0), capacidad_(0), ordenado_(true) {}

    /**
     * @brief Constructor para la initializer list
     *  @param lista
     **/
    Vector(std::initializer_list<tipodato> lista) : ordenado_(lista.size() <= 1) {
        tamano_ = capacidad_ = lista.size();
        datos_ = alloc.allocate(capacidad_);
        size_t i = 0;
        for (auto &dato : lista) {
            alloc_construct(alloc,&datos_[i++], dato);
        }
        verificarOrden();
    }

    /**
     * @brief Constructor que admite una capacidad y valor inicial
     * @param Capacidad
     * @param valor
     */
    explicit Vector(size_t Capacidad, const tipodato &valor = tipodato()) : ordenado_(true) {
        datos_ = alloc.allocate(Capacidad);
        tamano_ = Capacidad;
        capacidad_ = Capacidad;
        for (size_t i = 0; i < Capacidad; i++) {
            alloc_construct(alloc, &datos_[i], valor);
        }
    }

    /**
     * @brief Destructor de la clase Vector
     */
    ~Vector() {
        assert(tamano_ <= capacidad_);
        for (size_t i = 0; i < tamano_; ++i) {
            alloc_destroy(alloc, &datos_[i]);
        }
        if (datos_) {
            alloc.deallocate(datos_, capacidad_);
        }
    }

    /**
     * @brief Constructor para move semantics, admite otro vector como parametro
     * @param otro
     */
    Vector(Vector &&otro) noexcept {
        datos_ = otro.datos_;
        tamano_ = otro.tamano_;
        capacidad_ = otro.capacidad_;
        ordenado_ = otro.ordenado_;
        otro.datos_ = nullptr;
        otro.tamano_ = 0;
        otro.capacidad_ = 0;
        otro.ordenado_ = false;
    }

    /**
     * @brief Operador de asignacion
     * @param otro
     * @return
     */
    Vector &operator=(Vector &&otro) noexcept {
        if (this!=&otro) {
            for (size_t i = 0; i < tamano_; ++i) {
                alloc_destroy(alloc,&datos_[i]);
            }
            if (datos_) {
                alloc.deallocate(datos_, capacidad_);
            }
            datos_ = otro.datos_;
            tamano_ = otro.tamano_;
            capacidad_ = otro.capacidad_;
            ordenado_ = otro.ordenado_;
            otro.datos_ = nullptr;
            otro.tamano_ = 0;
            otro.capacidad_ = 0;
            otro.ordenado_ = false;
        }
        return *this;
    }

    /**
     * @brief Operador de copia
     * @param otro
     * @return
     */
    Vector& operator=(const Vector& otro) {
        if (this != &otro) {
            Vector temp(otro);
            swap(temp);
        }
        return *this;
    }

    /**
     * Constructor que permite la inclusion de otro vector como inicializador
     * @param otro
     */
    Vector(const Vector& otro) {
        datos_ = alloc.allocate(otro.capacidad_);
        tamano_ = otro.tamano_;
        capacidad_ = otro.capacidad_;
        ordenado_ = otro.ordenado_;
        for (size_t i = 0; i < tamano_; ++i) {
            alloc_construct(alloc, &datos_[i], otro.datos_[i]);
        }
    }

    //
    //  INICIO SECCION ITERADORES
    //

    /**
     * @class Iterator
     * @brief Define un iterador genérico para el recorrido de colecciones de datos.
     *
     * La clase Iterator proporciona herramientas básicas para iterar sobre elementos de una colección.
     * Permite moverse a través de los elementos sucesivos de la estructura de datos, facilitando
     * el acceso y manipulación de ellos.
     */
    struct Iterator {
        tipodato *ptr;

        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;

        constexpr Iterator() noexcept : ptr(nullptr) {}
        /**
         * @brief Constructor del iterador desde puntero.
         * @param p Puntero al elemento.
         */
        constexpr explicit Iterator(tipodato *p) noexcept : ptr(p) {}

        /**
         * @brief Constructor del iterador por copia.
         * @param otro Otro Iterator a copiar.
         *
         **/
        constexpr Iterator(const Iterator &otro) noexcept : ptr(otro.ptr) {}

        /**
         * @brief Operador para asignar un iterador a otro.
         * @param otro Iterador a asignar.
         * @return Referencia al iterador resultante.
         */
        constexpr Iterator &operator=(const Iterator &otro) noexcept {
            if (this != &otro) {
                ptr = otro.ptr;
            }
            return *this;
        }

        /**
         * @brief Operador de desrefenciación
         * @return
         */
        constexpr tipodato &operator*() const noexcept {
            return *ptr;
        }

        /**
         * @brief Operador flecha para acceder a miembros del objeto apuntado
         * @return
         */
        constexpr tipodato *operator->() const noexcept {
            return ptr;
        }

        //  Operaciones de incremento/decremento

        /**
         * @brief Operador de pre-incremento.
         * Avanza el iterador al siguiente elemento.
         * @return Referencia al iterador modificado.
         */
        constexpr Iterator &operator++() noexcept {
            ++ptr;
            return *this;
        }

        /**
         * @brief Operador de post-incremento.
         * Avanza el iterador al siguiente elemento, devolviendo una copia del estado previo.
         * @return Copia del iterador antes del incremento.
         */
        constexpr Iterator operator++(int) noexcept {
            Iterator aux = *this;
            ++ptr;
            return aux;
        }

        /**
        * @brief Operador de pre-decremento.
        * Retrocede el iterador al elemento anterior.
        * @return Referencia al iterador modificado.
        */
        constexpr Iterator &operator--() noexcept {
            --ptr;
            return *this;
        }

        /**
        * @brief Operador de post-decremento.
        * Retrocede el iterador al elemento anterior, devolviendo una copia del estado previo.
        * @return Copia del iterador antes del decremento.
        */
        constexpr Iterator operator--(int) noexcept {
            Iterator aux = *this;
            --ptr;
            return aux;
        }

        //  Operaciones Aritmeticas

        /**
         * @brief Operador de suma para avanzar el iterador.
         * @param n Número de posiciones a avanzar.
         * @return Nuevo iterador desplazado n posiciones.
         */
        constexpr Iterator operator+(std::ptrdiff_t n) const noexcept {
            return Iterator(ptr+n);
        }

        /**
        * @brief Operador de resta para retroceder el iterador.
        * @param n Número de posiciones a retroceder.
        * @return Nuevo iterador desplazado -n posiciones.
        */
        constexpr Iterator operator-(std::ptrdiff_t n) const noexcept {
            return Iterator(ptr-n);
        }

        /**
         * @brief Operador de suma-igual.
         * Avanza el iterador n posiciones.
         * @param n Número de posiciones a avanzar.
         * @return Referencia al iterador modificado.
         */
        constexpr Iterator &operator+=(std::ptrdiff_t n) noexcept {
            ptr+=n;
            return *this;
        }

        /**
         * @brief Operador de resta-igual.
         * Retrocede el iterador n posiciones.
         * @param n Número de posiciones a retroceder.
         * @return Referencia al iterador modificado.
         */
        constexpr Iterator &operator-=(std::ptrdiff_t n) noexcept {
            ptr-=n;
            return *this;
        }

        /**
         * @brief Operador de diferencia entre iteradores.
         * @param o Otro iterador.
         * @return Número de posiciones de diferencia entre los iteradores.
         */
        constexpr difference_type operator-(const Iterator &o) const noexcept {
            return ptr - o.ptr;
        }

        /**
         * @brief Acceso por índice relativo desde la posición actual del iterador.
         * @param indice Índice relativo.
         * @return Referencia al elemento en la posición indicada.
         */
        constexpr tipodato &operator[](std::ptrdiff_t indice) const noexcept {
            return *(ptr+indice);
        }

        //  Comparaciones
        /**
         * @brief Operador de desigualdad.
         * @param o Otro iterador.
         * @return true si los iteradores apuntan a distintas posiciones.
         */
        constexpr bool operator!=(const Iterator &o) const noexcept {
            return ptr != o.ptr;
        }
        /**
         * @brief Operador menor que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición anterior.
         */
        constexpr bool operator<(const Iterator &o) const noexcept {
            return ptr < o.ptr;
        }
        /**
         * @brief Operador de igualdad.
         * @param o Otro iterador.
         * @return true si los iteradores apuntan a la misma posición.
         */
        constexpr bool operator==(const Iterator &o) const noexcept {
            return ptr == o.ptr;
        }
        /**
         * @brief Operador mayor que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición posterior.
         */
        constexpr bool operator>(const Iterator &o) const noexcept {
            return ptr > o.ptr;
        }
        /**
         * @brief Operador menor o igual que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición anterior o igual.
         */
        constexpr bool operator<=(const Iterator &o) const noexcept {
            return ptr <= o.ptr;
        }
        /**
         * @brief Operador mayor o igual que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición posterior o igual.
         */
        constexpr bool operator>=(const Iterator &o) const noexcept {
            return ptr >= o.ptr;
        }
    };

    /**
     * @brief Metodo que apunta al inicio del vector para ser usado en un iterador
     * @return
     */
    Iterator begin() const {
        return Iterator(datos_);
    }

    /**
     * @brief Metodo que apunta al final del vector para ser usado en un iterador
     * @return
     */
    Iterator end() const {
        return Iterator(datos_+tamano_);
    }

    static_assert(std::input_iterator<Iterator>);

    friend constexpr Iterator operator+(std::ptrdiff_t n, const Iterator &it) {
        return it + n;
    }

    /**
     * @class ReverseIterator
     * @brief Iterador inverso para recorrer colecciones desde el final hacia el principio.
     *
     * La clase ReverseIterator proporciona funcionalidad completa de un iterador aleatorio inverso,
     * incluyendo soporte para aritmética de punteros, acceso aleatorio, comparación entre iteradores,
     * y conversión a iterador directo mediante el método `base()`.
     *
     * @tparam tipodato Tipo de dato que almacena la colección.
     */
    struct ReverseIterator {
        tipodato *ptr;  /// < Puntero al elemento actual

        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;

        constexpr ReverseIterator() noexcept : ptr(nullptr) {}
        /**
        * @brief Constructor explícito desde puntero.
        * @param p Puntero al elemento.
        */
        constexpr explicit ReverseIterator(tipodato *p) noexcept : ptr(p) {}

        /**
         * @brief Constructor por copia.
         * @param otro Otro ReverseIterator a copiar.
         */
        constexpr ReverseIterator(const ReverseIterator &otro) noexcept : ptr(otro.ptr) {}

        /**
         * @brief Operador de asignación.
         * @param otro ReverseIterator a asignar.
         * @return Referencia al iterador resultante.
         */
        constexpr ReverseIterator &operator=(const ReverseIterator &otro) noexcept {
            if (this!=&otro) {
                ptr = otro.ptr;
            }
            return *this;
        }

        /**
         * @brief Retorna un iterador directo apuntando al siguiente elemento (inverso de reverse).
         * @return Un objeto de tipo Iterator.
         */
        constexpr Iterator base() const {
            return Iterator(ptr+1);
        }

        /**
         * @brief Operador de desreferenciación.
         * @return Referencia al elemento actual.
         */
        constexpr tipodato &operator*() const noexcept {
            return *ptr;
        }
        /**
        * @brief Operador flecha para acceso a miembros.
        * @return Puntero al elemento actual.
        */
        constexpr tipodato &operator->() const noexcept {
            return &(*ptr);
        }

        /**
         * @brief Conversión a Iterator directo.
         */
        constexpr explicit operator Iterator() const noexcept {
            return Iterator(ptr + 1);
        }

        //  Operador de incremento
        constexpr ReverseIterator &operator++() noexcept {
            --ptr; return *this;
        }
        //  Operador de post incremento
        constexpr ReverseIterator operator++(int) noexcept {
            ReverseIterator aux = *this;
            --ptr;
            return aux;
        }
        //  Operador de decremento
        constexpr ReverseIterator &operator--() noexcept {
            ++ptr; return *this;
        }
        //  Operador de post decremento
        constexpr ReverseIterator operator--(int) noexcept {
            ReverseIterator aux = *this;
            ++ptr;
            return aux;
        }

        constexpr ReverseIterator operator+(std::ptrdiff_t n) const noexcept {
            return ReverseIterator(ptr-n);
        }
        constexpr ReverseIterator operator-(std::ptrdiff_t n) const noexcept {
            return ReverseIterator(ptr+n);
        }
        constexpr ReverseIterator &operator+=(std::ptrdiff_t n) noexcept {
            ptr-=n;
            return *this;
        }
        constexpr ReverseIterator &operator-=(std::ptrdiff_t n) noexcept {
            ptr+=n;
            return *this;
        }

        /**
        * @brief Diferencia entre dos iteradores.
        * @param o Otro ReverseIterator.
        * @return Número de elementos entre ellos.
        */
        constexpr difference_type operator-(const ReverseIterator &o) const noexcept {
            return o.ptr - ptr;
        }

        /**
         * @brief Acceso aleatorio a elementos mediante índice.
         * @param indice Índice relativo desde el iterador actual.
         * @return Referencia al elemento correspondiente.
         */
        constexpr tipodato &operator[](std::ptrdiff_t indice) const noexcept {
            return *(ptr-indice);
        }

        //  Operadores booleanos
        //  Operador distinto de
        constexpr bool operator!=(const ReverseIterator &o) const noexcept {
            return ptr != o.ptr;
        }
        //  Operador igual que
        constexpr bool operator==(const ReverseIterator &o) const noexcept {
            return ptr == o.ptr;
        }
        //  Operador menor que
        constexpr bool operator<(const ReverseIterator &o) const noexcept {
            return ptr > o.ptr;
        }
        //  Operador mayor que
        constexpr bool operator>(const ReverseIterator &o) const noexcept {
            return ptr < o.ptr;
        }
        //  Operador menor o igual que
        constexpr bool operator<=(const ReverseIterator &o) const noexcept {
            return ptr >= o.ptr;
        }
        //  Operador mayor o igual que
        constexpr bool operator>=(const ReverseIterator &o) const noexcept {
            return ptr <= o.ptr;
        }
    };
    /**
     * @brief Retorna un iterador inverso apuntando al último elemento.
     * @return ReverseIterator al final de la colección (último elemento).
     */
    ReverseIterator rbegin() const {
        return ReverseIterator(datos_+tamano_-1);
    }
    /**
     * @brief Retorna un iterador inverso apuntando antes del primer elemento.
     * @return ReverseIterator al "fin" lógico del recorrido inverso.
     */
    ReverseIterator rend() const {
        return ReverseIterator(datos_-1);
    }

    static_assert(std::input_iterator<ReverseIterator>);

    friend constexpr ReverseIterator operator+(std::ptrdiff_t n, const ReverseIterator &it) {
        return it + n;
    }

    //Const Iterator
    /**
     * @class ConstIterator
     * @brief Define un iterador constante genérico para el recorrido de colecciones de datos.
     *
     * La clase ConstIterator proporciona herramientas básicas para iterar sobre elementos de una colección.
     * Permite moverse a través de los elementos sucesivos de la estructura de datos, facilitando
     * el acceso y manipulación de ellos.
     */
    struct ConstIterator {
        const tipodato *ptr;    /// < Puntero al elemento actual
        //  Cosas requeridas para que el iterador funcione correctamente

        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = const tipodato*;
        using reference = const tipodato&;

        constexpr ConstIterator() noexcept : ptr(nullptr) {}
        constexpr explicit ConstIterator(const tipodato *p) noexcept : ptr(p) {}
        /**
        * @brief Constructor copia.
        * @param o Otro iterador constante.
        */
        constexpr ConstIterator(const ConstIterator &o) noexcept : ptr(o.ptr) {
            ptr = o.ptr;
        }

        constexpr explicit ConstIterator(const Iterator& it) noexcept : ptr(&(*it)) {}

        /**
         * @brief Operador de asignación.
         * @param o Otro iterador constante.
         * @return Referencia a este iterador.
         */
        constexpr ConstIterator &operator=(const ConstIterator &o) noexcept {
            if (this!=&o) ptr = o.ptr;
            return *this;
        }

        /**
         * @brief Operador de desreferenciación.
         * @return Referencia constante al dato apuntado.
         */
        constexpr const tipodato &operator*() const noexcept {
            return *ptr;
        }
        /**
         * @brief Operador flecha.
         * @return Puntero constante al dato apuntado.
         */
        constexpr const tipodato *operator->() const noexcept {
            return ptr;
        }

        //  Operador de incremento
        constexpr ConstIterator &operator++() noexcept {
            ++ptr;
            return *this;
        }
        //  Operador de incremento
        constexpr ConstIterator operator++(int) noexcept {
            ConstIterator aux = *this;
            ++ptr;
            return aux;
        }
        //  Operador de decremento
        constexpr ConstIterator &operator--() noexcept {
            --ptr;
            return *this;
        }
        //  Operador de decremento
        constexpr ConstIterator operator--(int) noexcept {
            ConstIterator aux = *this;
            --ptr;
            return aux;
        }

        constexpr ConstIterator operator+(std::ptrdiff_t n) const noexcept {
            return ConstIterator(ptr+n);
        }
        constexpr ConstIterator operator-(std::ptrdiff_t n) const noexcept {
            return ConstIterator(ptr-n);
        }
        constexpr ConstIterator &operator+=(std::ptrdiff_t n) noexcept {
            ptr+=n;
            return *this;
        }
        constexpr ConstIterator &operator-=(std::ptrdiff_t n) noexcept {
            ptr-=n;
            return *this;
        }

        /**
        * @brief Diferencia entre dos iteradores.
        * @param o Otro ReverseIterator.
        * @return Número de elementos entre ellos.
        */
        constexpr difference_type operator-(const ConstIterator &o) const noexcept {
            return ptr-o.ptr;
        }

        /**
         * @brief Operador de acceso por índice.
         * @param indice Desplazamiento desde el iterador actual.
         * @return Referencia constante al dato en la posición.
         */
        constexpr const tipodato &operator[](std::ptrdiff_t indice) const noexcept {
            return *(ptr+indice);
        }

        //  Operadores booleanos
        //  Operador igual que
        constexpr bool operator==(const ConstIterator &o) const noexcept {
            return ptr == o.ptr;
        }
        //  Operador distinto de
        constexpr bool operator!=(const ConstIterator &o) const noexcept {
            return ptr != o.ptr;
        }
        //  Operador menor que
        constexpr bool operator<(const ConstIterator &o) const noexcept {
            return ptr < o.ptr;
        }
        //  Operador mayor que
        constexpr bool operator>(const ConstIterator &o) const noexcept {
            return ptr > o.ptr;
        }
        //  Operador menor o igual que
        constexpr bool operator<=(const ConstIterator &o) const noexcept {
            return ptr >= o.ptr;
        }
        //  Operador mayor o igual que
        constexpr bool operator>=(const ConstIterator &o) const noexcept {
            return ptr <= o.ptr;
        }
    };

    /**
     * @brief Devuelve un iterador constante al inicio de la colección.
     * @return ConstIterator apuntando al primer elemento.
     */
    ConstIterator cbegin() const {
        return ConstIterator(datos_);
    }
    /**
     * @brief Devuelve un iterador constante al final de la colección.
     * @return ConstIterator apuntando justo después del último elemento.
     */
    ConstIterator cend() const {
        return ConstIterator(datos_+tamano_);
    }

    static_assert(std::input_iterator<ConstIterator>);

    friend constexpr ConstIterator operator+(std::ptrdiff_t n, const ConstIterator &it) {
        return it + n;
    }

    //  Const Reverse Iterator
    /**
     * @struct ConstReverseIterator
     * @brief Iterador constante reverso para estructuras con acceso aleatorio.
     *
     * Permite recorrer los elementos de la colección en orden inverso sin modificar su contenido.
     * Cumple con los requisitos de un iterador aleatorio.
     */
    struct ConstReverseIterator {
        tipodato *ptr;

        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;

        constexpr ConstReverseIterator() noexcept : ptr(nullptr) {}
        /**
         * @brief Constructor.
         * @param p Puntero al elemento actual (último de la colección).
         */
        constexpr explicit ConstReverseIterator(const tipodato *p) noexcept : ptr(p) {}

        /**
        * @brief Constructor copia.
        * @param otro Otro ConstReverseIterator.
        */
        constexpr ConstReverseIterator(const ConstReverseIterator &otro) noexcept : ptr(otro.ptr) {}

        constexpr explicit ConstReverseIterator(const ReverseIterator& it) noexcept : ptr(&(*it)) {}

        /**
         * @brief Operador de asignación.
         * @param otro Otro iterador a asignar.
         * @return Referencia a este iterador.
         */
        constexpr ConstReverseIterator &operator=(const ConstReverseIterator &otro) noexcept {
            if (this != &otro) {
                ptr = otro.ptr;
            }
            return *this;
        }

        /**
         * @brief Retorna un ConstIterator correspondiente a la posición "base" (siguiente elemento).
         * @return ConstIterator apuntando a `ptr + 1`.
         */
        constexpr ConstIterator base() const noexcept {
            return ConstIterator(ptr+1);
        }

        /**
         * @brief Conversión implícita a Iterator.
         * @return Iterator equivalente apuntando a `ptr + 1`.
         */
        constexpr explicit operator Iterator() const noexcept {
            return Iterator(ptr + 1);
        }

        /**
         * @brief Operador de desreferenciación.
         * @return Referencia constante al elemento actual.
         */
        constexpr const tipodato &operator*() const noexcept {
            return *ptr;
        }
        /**
        * @brief Operador flecha.
        * @return Puntero constante al elemento actual.
        */
        constexpr const tipodato &operator->() const noexcept {
            return &(*ptr);
        }

        //  Operador de incremento
        constexpr ConstReverseIterator &operator++() noexcept {
            --ptr; return *this;
        }
        //  Operador de incremento
        constexpr ConstReverseIterator operator++(int) noexcept {
            ConstReverseIterator aux = *this;
            --ptr;
            return aux;
        }
        //  Operador de decremento
        constexpr ConstReverseIterator &operator--() noexcept {
            ++ptr; return *this;
        }
        //  Operador de decremento
        constexpr ConstReverseIterator operator--(int) noexcept {
            ConstReverseIterator aux = *this;
            ++ptr;
            return aux;
        }

        constexpr ConstReverseIterator operator+(std::ptrdiff_t n) const noexcept {
            return ConstReverseIterator(ptr-n);
        }
        constexpr ConstReverseIterator operator-(std::ptrdiff_t n) const noexcept {
            return ConstReverseIterator(ptr+n);
        }
        constexpr ConstReverseIterator &operator+=(std::ptrdiff_t n) noexcept {
            ptr-=n;
            return *this;
        }
        constexpr ConstReverseIterator &operator-=(std::ptrdiff_t n) noexcept {
            ptr+=n;
            return *this;
        }

        constexpr difference_type operator-(const ConstReverseIterator &o) const noexcept {
            return ptr - o.ptr;
        }

        /**
         * @brief Operador de acceso por índice.
         * @param indice Desplazamiento desde el iterador actual (hacia atrás).
         * @return Referencia constante al elemento en la posición indicada.
         */
        constexpr const tipodato &operator[](std::ptrdiff_t indice) const noexcept {
            return *(ptr-indice);
        }

        //  Operadores booleanos
        //  Operador distinto de
        constexpr bool operator!=(const ConstReverseIterator &o) const noexcept {
            return ptr != o.ptr;
        }
        //  Operador igual a
        constexpr bool operator==(const ConstReverseIterator &o) const noexcept {
            return ptr == o.ptr;
        }
        //  Operador menor que
        constexpr bool operator<(const ConstReverseIterator &o) const noexcept {
            return ptr > o.ptr;
        }
        //  Operador mayor que
        constexpr bool operator>(const ConstReverseIterator &o) const noexcept {
            return ptr < o.ptr;
        }
        //  Operador menor o igual que
        constexpr bool operator<=(const ConstReverseIterator &o) const noexcept {
            return ptr >= o.ptr;
        }
        //  Operador mayor o igual que
        constexpr bool operator>=(const ConstReverseIterator &o) const noexcept {
            return ptr <= o.ptr;
        }
    };
    /**
     * @brief Devuelve un iterador constante reverso al final de la colección.
     * @return ConstReverseIterator apuntando al último elemento.
     */
    ConstReverseIterator crbegin() const {
        return ConstReverseIterator(datos_+tamano_-1);
    }
    /**
     * @brief Devuelve un iterador constante reverso al inicio de la colección.
     * @return ConstReverseIterator apuntando justo antes del primer elemento.
     */
    ConstReverseIterator crend() const {
        return ConstReverseIterator(datos_-1);
    }

    static_assert(std::input_iterator<ConstReverseIterator>);

    friend constexpr ConstReverseIterator operator+(std::ptrdiff_t n, const ConstReverseIterator &it) {
        return it + n;
    }

    constexpr Iterator begin() noexcept { return Iterator(datos_); }
    constexpr Iterator end() noexcept { return Iterator(datos_ + tamano_); }

    constexpr ConstIterator cbegin() noexcept { return ConstIterator(datos_); }
    constexpr ConstIterator cend() noexcept { return ConstIterator(datos_ + tamano_); }

    constexpr ReverseIterator rbegin() noexcept { return ReverseIterator(datos_ + tamano_ - 1); }
    constexpr ReverseIterator rend() noexcept { return ReverseIterator(datos_ - 1); }

    constexpr ConstReverseIterator crbegin() noexcept { return ConstReverseIterator(datos_ + tamano_ - 1); }
    constexpr ConstReverseIterator crend() noexcept { return ConstReverseIterator(datos_ - 1); }

    //
    //  FIN DE LA SECCION DE ITERADORES
    //

    /**
    * @brief Operador de salida.
    *
    * Permite imprimir el contenido del vector usando std::ostream.
    *
    * @param os Flujo de salida.
    * @param v Vector a imprimir.
    * @return Referencia al flujo de salida.
    */
    friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
        os << "[";
        for (size_t i = 0; i < v.tamano_; i++) {
            if (i > 0) os << ", ";
            os << v.datos_[i];
        }
        os << "]";
        return os;
    }

    /**
     * @brief Compara si dos vectores son iguales.
     *
     * Usa la función auxiliar areEqual para comparar los elementos.
     *
     * @param otro Vector a comparar.
     * @return true si son iguales, false en caso contrario.
     */
    bool operator==(const Vector &otro) const {
        if (tamano_ != otro.tamano_) return false;
        for (size_t i = 0; i < tamano_; i++) {
            if (!areEqual(datos_[i], otro.datos_[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Compara si dos vectores son diferentes.
     *
     * @param otro Vector a comparar.
     * @return true si son diferentes, false si son iguales.
     */
    bool operator!=(const Vector &otro) const {
        if (tamano_ != otro.tamano_) return true;
        for (size_t i = 0; i < tamano_; i++) {
            if (datos_[i] != otro.datos_[i]) return true;
        }
        return false;
    }

    bool operator<(const Vector& otro) const {
        return std::lexicographical_compare(
            datos_, datos_ + tamano_,
            otro.datos_, otro.datos_ + otro.tamano_
        );
    }

    bool operator<=(const Vector& otro) const {
        return *this < otro || *this == otro;
    }

    bool operator>(const Vector& otro) const {
        return !(*this <= otro);
    }

    bool operator>=(const Vector& otro) const {
        return !(*this < otro);
    }

    /**
     * @brief Construye un objeto en una posición específica del vector.
     *
     * Realiza construcción en el lugar.
     *
     * @tparam Args Tipos de los argumentos.
     * @param indice Posición en la que se construirá el objeto.
     * @param args Argumentos para el constructor de tipodato.
     */
    template<typename... Args>
    void emplace(size_t indice, Args&&... args) {
        if (indice > tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }

        if (tamano_ == capacidad_) {
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        }

        for (size_t i = tamano_; i > indice; --i) {
            alloc_construct(alloc, &datos_[i], std::move(datos_[i - 1]));
            alloc_destroy(alloc,&datos_[i - 1]);
        }

        alloc_construct(alloc, &datos_[indice], std::forward<Args>(args)...);

        ++tamano_;
        ordenado_ = false;
    }

    /**
     * @brief Construye un objeto al final del vector.
     *
     * Realiza construcción en el lugar.
     *
     * @tparam Args Tipos de los argumentos.
     * @param args Argumentos para el constructor de tipodato.
     */
    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (tamano_ == capacidad_) {
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        }

        alloc_construct(alloc, &datos_[tamano_], std::forward<Args>(args)...);
        ++tamano_;
        ordenado_ = false;
    }

    /**
     * @brief Redimensiona el vector.
     *
     * Cambia el tamaño del vector y rellena con valores si es necesario.
     *
     * @param nuevoTam Nuevo tamaño.
     * @param dato Valor con el que se rellenan los nuevos elementos (por defecto tipodato()).
     */
    void redimensionar(size_t nuevoTam, const tipodato &dato = tipodato()) {
        if (nuevoTam < tamano_) {
            for (size_t i = nuevoTam; i < tamano_; ++i) {
                alloc_destroy(alloc, &datos_[i]);
            }
            tamano_ = nuevoTam;

            if (tamano_ <= 1) {
                ordenado_ = true;
            }

            if (tamano_ < capacidad_ / 2) {
                reducirCapacidad();
            }
        } else if (nuevoTam > tamano_) {
            if (nuevoTam > capacidad_) {
                cambiarCapacidad(nuevoTam);
            }

            bool mantiene_orden = true;
            if (ordenado_ && tamano_ > 0) {
                mantiene_orden = !(dato < datos_[tamano_ - 1]);
            }

            for (size_t i = tamano_; i < nuevoTam; i++) {
                alloc_construct(alloc, &datos_[i], dato);
            }
            tamano_ = nuevoTam;

            if (tamano_ == 1) {
                ordenado_ = true;
            } else if (ordenado_ && !mantiene_orden) {
                ordenado_ = false;
            }
        }
    }

    /**
     * @brief Libera los recursos del vector.
     *
     * Reinicia tamaño y capacidad a cero.
     */
    void clear() {
        for (size_t i = 0; i < tamano_; ++i) {
            alloc_destroy(alloc,&datos_[i]);
        }
        if (datos_) {
            alloc.deallocate(datos_, capacidad_);
            datos_ = nullptr;
        }
        tamano_ = 0;
        capacidad_ = 0;
        ordenado_ = true;
    }

    /**
    * @brief Asegura que el contenedor tenga suficiente capacidad para al menos new_size elementos.
    *
    * Si la capacidad actual es menor que new_size, este método aumenta la capacidad
    * al máximo entre new_size o el doble de la capacidad actual, realocando y moviendo
    * los elementos existentes al nuevo espacio.
    *
    * @param new_size La capacidad mínima requerida.
    */
    void aumentarCapacidad(const size_t new_size) {
        if (new_size <= capacidad_) return;
        const size_t new_cap = std::max(new_size, capacidad_ * 2);
        tipodato* new_data = alloc.allocate(new_cap);

        for (size_t i = 0; i < tamano_; ++i) {
            alloc_construct(alloc, &new_data[i], std::move(datos_[i]));
            alloc_destroy(alloc,&datos_[i]);
        }

        if (datos_) {
            alloc.deallocate(datos_, capacidad_);
        }

        datos_ = new_data;
        capacidad_ = new_cap;
    }

    /**
    * @brief Añade al final del contenedor todos los elementos de un rango dado.
    *
    * Si el rango tiene un tamaño conocido, el contenedor reserva espacio suficiente
    * para todos los elementos nuevos. Luego, inserta cada elemento al final usando emplace_back.
    *
    * @tparam R Tipo que satisface std::ranges::input_range.
    * @param range El rango de elementos a añadir.
    */
    template <std::ranges::input_range R>
    void agregarRango(R&& range) {
        if constexpr (std::ranges::sized_range<R>) {
            auto count = std::ranges::distance(range);
            aumentarCapacidad(tamano_ + count);
        }

        for (auto&& val : range) {
            emplace_back(std::forward<decltype(val)>(val));
        }
        ordenado_ = false;
    }

    /**
    * @brief Inserta elementos de un rango dado en una posición específica del contenedor.
    *
    * Lanza std::out_of_range si pos es mayor que el tamaño actual.
    * Si el rango tiene un tamaño conocido, el contenedor reserva espacio suficiente
    * para insertar todos los elementos. Los elementos existentes desde pos en adelante
    * se mueven para hacer espacio, y los nuevos elementos se copian o mueven en ese hueco.
    *
    * @tparam R Tipo que satisface std::ranges::input_range.
    * @param pos La posición en la que insertar los nuevos elementos.
    * @param range El rango de elementos a insertar.
    * @return Iterador apuntando al primer elemento insertado.
    * @throws std::out_of_range si pos > tamaño actual.
    */
    template <std::ranges::input_range R>
    Iterator insertarRango(size_t pos, R&& range) {
        static_assert(std::convertible_to<std::ranges::range_reference_t<R>, tipodato>,
                      "El tipo de elemento debe ser convertible a tipodato");

        if (pos > tamano_) {
            throw std::out_of_range("insert_range: posicion fuera de limites");
        }

        bool self_insertion = false;
        if constexpr (std::is_lvalue_reference_v<R> && std::is_same_v<std::remove_cvref_t<R>, Vector>) {
            const Vector* ptr = &range;
            if (ptr == this) {
                self_insertion = true;
            }
        }

        if (self_insertion) {
            Vector<tipodato> temp_storage;
            for (auto&& val : range) {
                temp_storage.push_back(val);
            }
            size_t insert_count = temp_storage.size();

            aumentarCapacidad(tamano_ + insert_count);

            for (size_t i = tamano_; i-- > pos;) {
                alloc_construct(alloc, &datos_[i + insert_count], std::move(datos_[i]));
                alloc_destroy(alloc,&datos_[i]);
            }

            size_t idx = pos;
            for (auto&& val : temp_storage) {
                alloc_construct(alloc, &datos_[idx++], val);
            }

            tamano_ += insert_count;

        } else {
            size_t insert_count = 0;
            if constexpr (std::ranges::sized_range<R>) {
                insert_count = std::ranges::distance(range);
            } else {
                for (auto it = std::ranges::begin(range); it != std::ranges::end(range); ++it) ++insert_count;
            }

            aumentarCapacidad(tamano_ + insert_count);

            for (size_t i = tamano_; i-- > pos;) {
                alloc_construct(alloc, &datos_[i + insert_count], std::move(datos_[i]));
                alloc_destroy(alloc,&datos_[i]);
            }

            size_t idx = pos;
            for (auto&& val : range) {
                alloc_construct(alloc, &datos_[idx++], std::forward<decltype(val)>(val));
            }

            tamano_ += insert_count;
        }
        ordenado_ = false;
        return Iterator(datos_ + pos);
    }

    /**
     * @brief Agrega un valor al final del vector.
     *
     * @param dato Valor a agregar.
     */
    void agregarFinal(tipodato&& dato) {
        if (ordenado_ && !empty() && dato < datos_[tamano_ - 1]) {
            ordenado_ = false;
        }
        if (tamano_ == capacidad_)
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        alloc_construct(alloc,&datos_[tamano_], std::move(dato));
        ++tamano_;
    }

    /**
    * @brief Agrega un valor al final del vector.
    *
    * @param dato Valor a agregar.
    */
    void agregarFinal(const tipodato& dato) {
        if constexpr (requires { dato < datos_[tamano_ - 1]; }) {
            if (ordenado_ && !empty() && dato < datos_[tamano_ - 1]) {
                ordenado_ = false;
            }
        }
        if (tamano_ == capacidad_)
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        alloc_construct(alloc,&datos_[tamano_], dato);
        ++tamano_;
    }

    /**
     * @brief Obtiene el último elemento del vector.
     *
     * @return Referencia al último elemento.
     * @throws std::out_of_range si el vector está vacío.
     */
    tipodato &ultimo() {
        if (tamano_==0) throw std::out_of_range("No hay elementos en el vector");
        return datos_[tamano_ - 1];
    }

    /**
     * @brief Accede a un valor por índice (no verificado).
     *
     * @param indice Índice del valor.
     * @return Referencia al valor.
     */
    tipodato &operator[](size_t indice) {
        ordenado_ = false;
        return datos_[indice];
    }

    /**
    * @brief Accede a un valor por índice (no verificado).
    *
    * @param indice Índice del valor.
    * @return Referencia constante al valor.
    */
    const tipodato &operator[](size_t indice) const {
        return datos_[indice];
    }

    /**
     * @brief Devuelve el valor ubicado en un índice específico.
     *
     * Verifica los límites del vector.
     *
     * @param indice Índice del valor.
     * @return Referencia al valor.
     * @throws std::out_of_range si el índice es inválido.
     */
    tipodato &en(size_t indice) {
        if (indice>=tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }
        return datos_[indice];
    }

    /**
     * @brief Devuelve el valor ubicado en un índice específico.
     *
     * Verifica los límites del vector.
     *
     * @param indice Índice del valor.
     * @return Referencia al valor.
     * @throws std::out_of_range si el índice es inválido.
     */
    const tipodato &en(size_t indice) const {
        if (indice>=tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }
        return datos_[indice];
    }
    /**
     * @brief Devuelve el valor ubicado en un índice específico.
     *
     * Verifica los límites del vector.
     *
     * @param indice Índice del valor.
     * @return Referencia al valor.
     * @throws std::out_of_range si el índice es inválido.
     */
    tipodato &at(size_t indice) {
        if (indice>=tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }
        return datos_[indice];
    }
    /**
     * @brief Devuelve el valor ubicado en un índice específico.
     *
     * Verifica los límites del vector.
     *
     * @param indice Índice del valor.
     * @return Referencia al valor.
     * @throws std::out_of_range si el índice es inválido.
     */
    const tipodato &at(size_t indice) const {
        if (indice>=tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }
        return datos_[indice];
    }

    /**
     * @brief Verifica si el vector está ordenado.
     * @return true si el vector está ordenado.
     */
    [[nodiscard]] bool estaOrdenado() const {
        return ordenado_;
    }

    /**
     * @brief Verifica si el vector está vacío.
     *
     * @return true si el tamaño es 0, false en caso contrario.
     */
    [[nodiscard]] bool vacio() const {
        return tamano_ == 0;
    }

    /**
    * @brief Devuelve el tamaño actual del vector.
    *
    * @return Tamaño del vector.
    */
    [[nodiscard]] size_t obtenerTamano() const {
        return tamano_;
    }

    /**
     * @brief Devuelve la capacidad actual del vector.
     *
     * @return Capacidad del vector.
     */
    [[nodiscard]] size_t obtenerCapacidad() const {
        return capacidad_;
    }
    /**
    * @brief Devuelve la capacidad restante disponible en el vector.
    * @return Número de elementos que se pueden insertar antes de redimensionar.
    */
    [[nodiscard]] size_t capacidadLibre() const {
        return capacidad_ - tamano_;
    }

private:
    /**
    * @brief Cambia la capacidad del vector.
    *
    * Si la nueva capacidad es menor o igual que la actual, no hace nada.
    *
    * @param nuevaCapacidad Nueva capacidad deseada.
    */
    void cambiarCapacidad(size_t nuevaCapacidad = 0) {
        if (nuevaCapacidad == 0) {
            nuevaCapacidad = capacidad_ == 0 ? 1 : capacidad_ * 2;
        }

        if (nuevaCapacidad <= capacidad_ && datos_ != nullptr) {
            return;
        }

        tipodato* nuevo = alloc.allocate(nuevaCapacidad);

        try {
            std::uninitialized_move(datos_, datos_ + tamano_, nuevo);
        } catch (...) {
            alloc.deallocate(nuevo, nuevaCapacidad);
            throw;
        }

        if constexpr (!std::is_trivially_destructible_v<tipodato>) {
            std::destroy(datos_, datos_ + tamano_);
        }

        if (datos_) {
            alloc.deallocate(datos_, capacidad_);
        }

        datos_ = nuevo;
        capacidad_ = nuevaCapacidad;
    }

    void verificarOrden() {
        if (tamano_ <= 1) {
            ordenado_ = true;
            return;
        }

        if (!ordenado_) {
            return;
        }

        for (size_t i = 1; i < tamano_; ++i) {
            if (datos_[i] < datos_[i-1]) {
                ordenado_ = false;
                return;
            }
        }
        ordenado_ = true;
    }

public:
    /**
    * @brief Elimina la primera ocurrencia de un valor.
    *
    * Si no se encuentra, no hace nada.
    *
    * @param dato Valor a eliminar.
    */
    void eliminarDato(const tipodato& dato) {
        for (size_t i = 0; i < tamano_; i++) {
            if (datos_[i] == dato) {
                eliminar(i);
                return;
            }
        }
    }

    /**
     * @brief Muestra los elementos del vector por consola.
     */
    void mostrar() const {
        for (size_t i = 0; i < tamano_; i++) {
            std::cout << datos_[i];
            if (i+1!=tamano_) {
                std::cout<<" - ";
            }
        }
        std::cout << "\n";
    }

    /**
     * @brief Vacía el vector sin liberar memoria.
     *
     * Solo ajusta el tamaño a cero.
     */
    void vaciar() {
        for (size_t i = 0; i < tamano_; ++i) {
            alloc_destroy(alloc,&datos_[i]);
        }
        tamano_ = 0;
        ordenado_ = true;
    }

    /**
     * @brief Elimina el último elemento del vector.
     *
     * @throws std::out_of_range si el vector está vacío.
     */
    void eliminarFinal() {
        if (tamano_ == 0) {
            throw std::out_of_range("No hay elementos en el vector");
        }
        --tamano_;
        alloc_destroy(alloc,&datos_[tamano_]);
    }

    /**
     * @brief Elimina el elemento apuntado por un iterador.
     *
     * Usa if constexpr para elegir entre memmove o std::move según el tipo.
     *
     * @param it Iterador al elemento a eliminar.
     * @return Iterador al siguiente elemento.
     * @throws std::out_of_range si el iterador está fuera de rango.
     */
    template<typename U = tipodato>
    Iterator erase(Iterator it) {
        if (it.ptr < datos_ || it.ptr >= datos_ + tamano_)
            throw std::out_of_range("Iterador fuera de rango");

        size_t idx = it.ptr - datos_;

        alloc_destroy(alloc, &datos_[idx]);

        if constexpr (std::is_trivially_copyable_v<tipodato>) {
            std::memmove(&datos_[idx], &datos_[idx + 1], sizeof(tipodato) * (tamano_ - idx - 1));
        } else {
            for (size_t i = idx; i < tamano_ - 1; ++i) {
                alloc_construct(alloc, &datos_[i], std::move(datos_[i + 1]));
                alloc_destroy(alloc, &datos_[i + 1]);
            }
        }
        --tamano_;
        return Iterator(datos_ + idx);
    }

    /**
     * @brief Elimina un rango de elementos.
     *
     * Usa if constexpr para optimizar según el tipo de dato.
     *
     * @param first Iterador al primer elemento a eliminar.
     * @param last Iterador al siguiente al último.
     * @return Iterador al siguiente elemento tras el borrado.
     */
    template<typename U = tipodato>
    Iterator erase(Iterator first, Iterator last) {
        if (first == last) return first;
        size_t start = first - begin(), end = last - begin(), count = end - start;

        if constexpr (std::is_trivially_copyable_v<tipodato>) {
            std::memmove(&datos_[start], &datos_[end], sizeof(tipodato) * (tamano_ - end));
        } else {
            for (size_t i = 0; i < tamano_ - end; ++i) {
                alloc_construct(alloc, &datos_[start + i], std::move(datos_[end + i]));
                alloc_destroy(alloc, &datos_[end + i]);
            }
        }
        tamano_ -= count;
        return Iterator(datos_ + start);
    }

    /**
    * @brief Inserta un valor en el índice especificado.
    *
    * Usa if constexpr para optimizar el movimiento o copia según el tipo.
    *
    * @param indice Índice donde se insertará el valor.
    * @param dato Valor a insertar.
    * @throws std::out_of_range si el índice está fuera de rango.
    */
    void insertar(size_t indice, const tipodato& dato) {
        if (indice > tamano_) throw std::out_of_range("Indice fuera de rango");
        if (tamano_ == capacidad_) cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);

        if constexpr (std::is_trivially_copyable_v<tipodato>) {
            std::memmove(&datos_[indice + 1], &datos_[indice], sizeof(tipodato) * (tamano_ - indice));
            alloc_construct(alloc, &datos_[indice], dato);
        } else {
            for (size_t i = tamano_; i > indice; --i) {
                alloc_construct(alloc, &datos_[i], std::move(datos_[i - 1]));
                alloc_destroy(alloc, &datos_[i - 1]);
            }
            alloc_construct(alloc, &datos_[indice], dato);
        }
        ++tamano_;
        ordenado_ = false;
    }

    /**
    * @brief Elimina el elemento ubicado en el índice dado.
    *
    * @param indice Índice del elemento a eliminar.
    * @throws std::out_of_range si el índice está fuera de rango.
    */

    void eliminar(size_t indice) {
        if (indice >= tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }

        alloc_destroy(alloc, &datos_[indice]);

        for (size_t i = indice; i < tamano_ - 1; ++i) {
            alloc_construct(alloc, &datos_[i], std::move(datos_[i + 1]));
            alloc_destroy(alloc, &datos_[i + 1]);
        }

        --tamano_;
    }

    /**
    * @brief Verifica si el vector contiene un valor.
    *
    * @param dato Valor a buscar.
    * @return true si se encuentra el valor, false en caso contrario.
    */
    bool contiene(const tipodato &dato) const {
        for (size_t i = 0; i < tamano_; i++) {
            if (datos_[i] == dato) {
                return true;
            }
        }
        return false;
    }

    /**
    * @brief Busca un valor en el vector.
    *
    * @param dato Valor a buscar.
    * @return Índice del valor si se encuentra, -1 en caso contrario.
    */
    int buscar(const tipodato &dato) const {
        for (size_t i = 0; i < tamano_; i++) {
            if (datos_[i] == dato) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    /**
    * @brief Obtiene el valor en una posición determinada.
    *
    * @param indice Índice del valor a obtener.
    * @return Copia del valor en la posición indicada.
    * @throws std::out_of_range si el índice es inválido.
    */
    tipodato obtener(const int indice) const {
        if (indice<0||indice>=tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }
        return datos_[indice];
    }

    /**
    * @brief Reserva una nueva capacidad para el vector.
    *
    * No reduce la capacidad si nuevaCapacidad <= capacidad.
    *
    * @param nuevaCapacidad Nueva capacidad deseada.
    */
    void reservar(size_t nuevaCapacidad) {
        if (nuevaCapacidad > capacidad_) {
            cambiarCapacidad(nuevaCapacidad);
        }
    }

    /**
    * @brief Reduce la capacidad del vector a su tamaño actual.
    *
    * Libera memoria sobrante si existe.
    */
    void reducirCapacidad() {
        if (capacidad_ > tamano_) {
            tipodato* nuevo = alloc.allocate(tamano_);
            size_t i = 0;
            try {
                for (; i < tamano_; ++i) {
                    alloc_construct(alloc, &nuevo[i], std::move_if_noexcept(datos_[i]));
                }
            } catch (...) {
                for (size_t j = 0; j < i; ++j) {
                    alloc_destroy(alloc, &nuevo[j]);
                }
                alloc.deallocate(nuevo, tamano_);
                throw;
            }
            for (size_t j = 0; j < tamano_; ++j) {
                alloc_destroy(alloc, &datos_[j]);
            }
            if (datos_) {
                alloc.deallocate(datos_, capacidad_);
            }
            datos_ = nuevo;
            capacidad_ = tamano_;
        }
    }

    /**
    * @brief Intercambia el contenido de este vector con otro.
    * @param otro Vector con el cual se intercambiarán los datos.
    */
    void intercambiar(Vector &otro) {
        std::swap(datos_, otro.datos_);
        std::swap(tamano_, otro.tamano_);
        std::swap(capacidad_, otro.capacidad_);
        std::swap(ordenado_, otro.ordenado_);
    }

    /**
    * @brief Devuelve una referencia al primer elemento del vector.
    * @return Referencia al elemento frontal.
    * @throws std::out_of_range Si el vector está vacío.
    */
    tipodato &frente() {
        if (tamano_==0) throw std::out_of_range("No hay elementos en el vector");
        return datos_[0];
    }

    /**
    * @brief Devuelve una referencia constante al primer elemento del vector.
    * @return Referencia constante al elemento frontal.
    * @throws std::out_of_range Si el vector está vacío.
    */
    const tipodato &frente() const {
        if (tamano_==0) throw std::out_of_range("No hay elementos en el vector");
        return datos_[0];
    }

    /**
     * @brief Devuelve una referencia al último elemento del vector.
     * @return Referencia al elemento trasero.
     * @throws std::out_of_range Si el vector está vacío.
     */
    tipodato &atras() {
        if (tamano_==0) throw std::out_of_range("No hay elementos en el vector");
        return datos_[tamano_-1];
    }

    /**
    * @brief Devuelve una referencia constante al último elemento del vector.
    * @return Referencia constante al elemento trasero.
    * @throws std::out_of_range Si el vector está vacío.
    */
    const tipodato &atras() const {
        if (tamano_==0) throw std::out_of_range("No hay elementos en el vector");
        return datos_[tamano_-1];
    }

    /**
    * @brief Invierte el orden de los elementos en el vector.
    */
    void invertir() {
        for (size_t i = 0; i < tamano_/2; i++) {
            tipodato aux = datos_[i];
            datos_[i] = datos_[tamano_-i-1];
            datos_[tamano_-i-1] = aux;
        }
        ordenado_ = false;
    }

    /**
    * @brief Ordena el vector usando el algoritmo de burbuja (O(n^2)).
    */
    void ordenarBurbuja() {
        for (size_t i = 0; i < tamano_; i++) {
            for (size_t j = i+1; j < tamano_; j++) {
                if (datos_[i] > datos_[j]) {
                    tipodato aux = datos_[i];
                    datos_[i] = datos_[j];
                    datos_[j] = aux;
                }
            }
        }
        ordenado_ = true;
    }

    /**
    * @brief Ordena el vector usando el algoritmo IntroSort (a través de std::sort).
    * Complejidad temporal: O(n log n)
    */
    void ordenar() {
        std::sort(begin(),end());
        ordenado_ = true;
    }

    /**
    * @brief Crea un subvector a partir de un rango de índices.
    * @param desde Índice de inicio (inclusive).
    * @param hasta Índice de fin (exclusive).
    * @return Subvector con los elementos seleccionados.
    * @throws std::out_of_range Si los índices no son válidos.
    */
    Vector subvector(size_t desde, size_t hasta) const {
        if (desde >= hasta || hasta > tamano_ || desde > tamano_) {
            return Vector{};
        }
        Vector nuevo;
        nuevo.reservar(hasta - desde);
        for (size_t i = desde; i < hasta; ++i) {
            nuevo.agregarFinal(datos_[i]);
        }
        return nuevo;
    }

    /**
    * @brief Reemplaza todas las apariciones de un dato por otro.
    * @param dato Valor a reemplazar.
    * @param nuevo Nuevo valor.
    */
    void reemplazar(const tipodato &dato, const tipodato &nuevo) {
        for (size_t i = 0; i < tamano_; i++) {
            if (datos_[i] == dato) {
                datos_[i] = nuevo;
            }
        }
        ordenado_ = false;
    }

    /**
    * @brief Elimina todos los elementos duplicados del vector.
    */
    void eliminarDuplicados() {
        if (!estaOrdenado()) {
            std::sort(begin(), end());
            ordenado_ = true;
        }
        erase(std::unique(begin(), end()), end());
    }

    /**
    * @brief Cuenta cuántas veces aparece un dato en el vector.
    * @param dato Valor a contar.
    * @return Número de apariciones del valor.
    */
    size_t contar(const tipodato &dato) const {
        size_t contador = 0;
        for (size_t i = 0; i < tamano_; i++) {
            if (datos_[i] == dato) {
                ++contador;
            }
        }
        return contador;
    }

    /**
    * @brief Reduce la capacidad si el tamaño es menor a la mitad de la capacidad actual.
    */
    void ajustarCapacidad() {
        if (tamano_<capacidad_/2) {
            reducirCapacidad();
        }
    }

    /**
    * @brief Intercambia los elementos en los índices dados.
    * @param i Índice del primer elemento.
    * @param j Índice del segundo elemento.
    * @throws std::out_of_range Si alguno de los índices es inválido.
    */
    void intercambiarIndices(size_t i, size_t j) {
        if (i>=tamano_||j>=tamano_) throw std::out_of_range("Indice fuera de rango");
        tipodato aux = datos_[i];
        datos_[i] = datos_[j];
        datos_[j] = aux;
        ordenado_ = false;
    }

    /**
    * @brief Inserta otro vector a partir de un índice específico.
    * @param indice Índice donde se insertarán los datos.
    * @param v Vector a insertar.
    * @throws std::out_of_range Si el índice es inválido.
    */
    void insertarVector(size_t indice, const Vector &v) {
        if (indice > tamano_) throw std::out_of_range("Indice fuera de rango");
        Vector copia = v;

        while (tamano_ + copia.tamano_ > capacidad_) {
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        }

        for (size_t i = tamano_; i > indice; --i) {
            alloc_destroy(alloc, &datos_[i + copia.tamano_ - 1]);
            alloc_construct(alloc, &datos_[i + copia.tamano_ - 1], std::move(datos_[i - 1]));
            alloc_destroy(alloc, &datos_[i - 1]);
        }

        for (size_t i = 0; i < copia.tamano_; ++i) {
            alloc_construct(alloc, &datos_[indice + i], copia.datos_[i]);
        }

        tamano_ += copia.tamano_;
        ordenado_ = false;
    }

    /** @name Métodos compatibles con std::vector
    *Métodos alternativos con nombres en inglés para facilitar la interoperabilidad.
    */
    ///@{

    /**
    * @brief Resizes the vector, optionally filling with a value.
    * @param newSize New size of the vector.
    * @param value Value used to initialize the new elements (if any).
    */
    void resize(size_t newSize, const tipodato &value = tipodato()) {
        redimensionar(newSize, value);
    }
    /**
    * @brief Adds a value at the end of the vector.
    * @param value Value to be added.
    */
    void push_back(const tipodato &value) {
        agregarFinal(value);
    }
    /**
    * @brief Removes the last element of the vector.
    */
    void pop_back() {
        eliminarFinal();
    }
    /**
    * @brief Reserves a new capacity for the vector.
    * @param newCapacity Minimum desired capacity.
    */
    void reserve(size_t newCapacity) {
        reservar(newCapacity);
    }

    /**
    * @brief Inserts a value at a position using iterators.
    * @param pos Destination position.
    * @param val Value to insert.
    * @return Iterator pointing to the newly inserted element.
    */
    Iterator insert(Iterator pos, const tipodato& val) {
        size_t idx = pos - begin();
        insert(idx, val);
        ordenado_ = false;
        return begin() + idx;
    }
    /**
    * @brief Inserts a value at the specified index.
    * @param index Index where the value will be inserted.
    * @param value Value to insert.
    */
    void insert(size_t index, const tipodato &value) {
        insertar(index,value);
    }
    /**
    * @brief Removes the element at the specified index.
    * @param index Index of the element to be removed.
    */
    void erase(size_t index) {
        eliminar(index);
    }

    template<typename InputIterator>
    Iterator insert(Iterator pos, InputIterator first, InputIterator last) {
        size_t idx = pos - begin();
        size_t count = std::distance(first, last);

        if (first >= begin() && first < end()) {
            std::vector<tipodato> temp(first, last);
            return insert(begin() + idx, temp.begin(), temp.end());
        }

        if (tamano_ + count > capacidad_) {
            cambiarCapacidad(std::max(tamano_ + count, capacidad_ == 0 ? count : capacidad_ * 2));
        }

        for (size_t i = tamano_; i > idx; --i) {
            alloc_construct(alloc, &datos_[i + count - 1], std::move(datos_[i - 1]));
            alloc_destroy(alloc, &datos_[i - 1]);
        }

        size_t insert_pos = idx;
        for (auto it = first; it != last; ++it, ++insert_pos) {
            alloc_construct(alloc, &datos_[insert_pos], *it);
        }

        tamano_ += count;
        ordenado_ = false;
        return begin() + idx;
    }
    /**
    * @brief Swaps the contents with another vector.
    * @param other Vector with which to swap contents.
    */
    void swap(Vector &other) noexcept {
        intercambiar(other);
    }
    /**
    * @brief Direct access to the internal memory block.
    * @return Pointer to the data.
    */
    tipodato *data() noexcept {
        return datos_;
    }
    /**
    * @brief Constant direct access to the internal memory block.
    * @return Constant pointer to the data.
    */
    const tipodato *data() const noexcept {
        return datos_;
    }
    /**
    * @brief Returns a reference to the first element.
    * @return Reference to the first element.
    */
    tipodato &front() {
        return frente();
    }
    /**
    * @brief Returns a reference to the last element.
    * @return Reference to the last element.
    */
    tipodato &back() {
        return atras();
    }
    /**
    * @brief Reduces the capacity to match the current size.
    */
    void shrink_to_fit() {
        reducirCapacidad();
    }
    /**
    * @brief Returns the number of elements currently stored in the vector.
    * @return Size of the vector.
    */
    [[nodiscard]] size_t size() const {
        return tamano_;
    }
    /**
    * @brief Returns the current capacity of the vector.
    * @return Capacity of the vector.
    */
    [[nodiscard]] size_t capacity() const {
        return capacidad_;
    }
    /**
    * @brief Checks whether the vector is empty.
    * @return true if the vector is empty, false otherwise.
    */
    [[nodiscard]] bool empty() const {
        return tamano_ == 0;
    }
    /**
     * @brief Checks whether the vector is sorted.
     * @return true if the vector is sorted.
     */
    [[nodiscard]] bool isSorted() const {
        return ordenado_;
    }
    /**
    * @brief Returns the maximum number of elements that the vector can theoretically hold.
    * @return Maximum size supported by the system.
    */
    size_t static max_size() {
        return std::numeric_limits<size_t>::max();
    }
    /**
    * @brief Prints all elements in the vector to the console.
    */
    void display() const {
        mostrar();
    }
    /**
    * @brief Removes all duplicate values in the vector.
    */
    void remove_duplicates() {
        eliminarDuplicados();
    }
    /**
    * @brief Reverses the order of the elements in the vector.
    */
    void reverse() {
        invertir();
    }
    /**
    * @brief Swaps the values at two specified indices.
    * @param indexA Index of the first element.
    * @param indexB Index of the second element.
    * @return 1 if swapped successfully, 0 or error code otherwise.
    */
    int swap_indices(size_t indexA, size_t indexB) {
        return intercambiarIndices(indexA, indexB);
    }
    /**
    * @brief Replaces all occurrences of a value with another.
    * @param oldValue Value to be replaced.
    * @param newValue Replacement value.
    * @return Number of elements replaced.
    */
    tipodato replace_all(tipodato oldValue, tipodato newValue) {
        return reemplazar(oldValue, newValue);
    }
    /**
    * @brief Creates a subvector between two indices (inclusive from, exclusive to).
    * @param from Starting index (inclusive).
    * @param to Ending index (exclusive).
    * @return A new Vector containing the selected range.
    */
    Vector slice(const size_t from, const size_t to) const {
        return subvector(from, to);
    }
    /**
    * @brief Sorts the elements in ascending order using std::sort.
    */
    void sort() {
        ordenar();
    }
    /**
    * @brief Sorts the elements using bubble sort (not recommended for large vectors).
    */
    void bubble_sort() {
        ordenarBurbuja();
    }

    void grow_to_fit(const size_t newSize) {
        aumentarCapacidad(newSize);
    }

    template <std::ranges::input_range R>
    void append_range(R&& range) {
        agregarRango(std::forward<R>(range));
    }

    template <std::ranges::input_range R>
    Iterator insert_range(size_t pos, R&& range) {
        return insertarRango(pos, std::forward<R>(range));
    }

    ///@}

    //
    // [DEBUG]
    //

    // tipodato* raw() const {
    //     return datos;
    // }

};

template<typename T, typename Alloc>
auto borrow_view(const Vector<T, Alloc>& vec) {
    return std::ranges::subrange(vec.begin(), vec.end());
}

#endif //CPPVECTOR_H
