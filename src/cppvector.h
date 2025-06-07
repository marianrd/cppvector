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

#if __cplusplus>=201703L
#include <ranges>
/**
* @brief Compara la igualdad entre dos valores en C++17 y posteriores.
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
#endif
#if __cplusplus>=201103L && __cplusplus<201703L
/**
 * @brief Compara la igualdad entre dos valores en C++11 y C++14.
 *
 * Esta implementación también cuenta con especializaciones para datos
 * de tipo flotante y no flotante.
 *
 * @tparam T Tipo de datos a comparar.
 * @param a Primer valor.
 * @param b Segundo valor.
 * @return `true` si los valores son iguales, `false` en caso contrario.
 *
 * **Notas:**
 * - Para datos que **no son de punto flotante**, compara con `==`.
 * - Para datos **de punto flotante**, maneja la comparación usando
 *   `std::numeric_limits<T>::epsilon()` para tomar en cuenta errores numéricos.
 */
template<typename T>
typename std::enable_if<!std::is_floating_point<T>::value, bool>::type
areEqual(const T& a, const T& b) {
    return a == b;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
areEqual(const T& a, const T& b) {
    return std::abs(a - b) <= std::numeric_limits<T>::epsilon();
}
#endif
#if __cplusplus<201103L
#include <cmath>
/**
 * @brief Verifica si un tipo es de punto flotante.
 *
 * Esta plantilla de estructura define un valor constante que indica si
 * un tipo `T` es un tipo flotante (`float`, `double`, `long double`).
 *
 * @tparam T Tipo a analizar.
 */
template<typename T>
struct is_floating_point {
    static const bool value = false;
};

template<>
struct is_floating_point<float> {
    static const bool value = true;
};

template<>
struct is_floating_point<double> {
    static const bool value = true;
};

template<>
struct is_floating_point<long double> {
    static const bool value = true;
};

/**
 * @brief Implementación para comparar igualdad de datos.
 *
 * Esta plantilla maneja la comparación según si el tipo es de punto flotante
 * o no.
 *
 * @tparam T Tipo de datos.
 * @tparam IsFloat `true` si el tipo es de punto flotante, `false` en caso contrario.
 */

template<typename T, bool IsFloat>
struct AreEqualImpl {
    static bool equal(const T& a, const T& b) {
        return a == b;
    }
};

// Especialización para tipo flotante
template<typename T>
struct AreEqualImpl<T, true> {
    /**
     * @brief Compara igualdad para datos que no son de punto flotante.
     *
     * @param a Primer valor.
     * @param b Segundo valor.
     * @return `true` si los valores son iguales, `false` en caso contrario.
     */
    static bool equal(const T& a, const T& b) {
        return std::fabs(a - b) <= std::numeric_limits<T>::epsilon();
    }
};

template<typename T>
bool areEqual(const T& a, const T& b) {
    return AreEqualImpl<T, is_floating_point<T>::value>::equal(a, b);
}

#endif

// Inicio vector dinamico

/**
* @struct Vector
* @brief Una implementacion de un vector dinamico similar a std::vector que soporta algoritmos STL.
*
* La estructura Vector soporta la asignacion y manejo de un array de elementos de tipo 'tipodato'.
* Proporciona diferentes metodos de inicializacion, iteradores para el recorrido del vector y soporte
* para move semantics en las versiones superiores a C++11.
*
* @tparam tipodato Tipo de dato almacenado
*/
template<typename tipodato, typename Allocator = std::allocator<tipodato>>
struct Vector {

#if __cplusplus >= 201703L
    template<typename Alloc, typename Ptr, typename... Args>
    void alloc_construct(Alloc& alloc, Ptr ptr, Args&&... args) {
        std::allocator_traits<Alloc>::construct(alloc, ptr, std::forward<Args>(args)...);
    }

    template<typename Alloc, typename Ptr>
    void alloc_destroy(Alloc& alloc, Ptr ptr) {
        std::allocator_traits<Alloc>::destroy(alloc, ptr);
    }

#elif __cplusplus >= 201103L
    template<typename Alloc, typename Ptr, typename... Args>
    void alloc_construct(Alloc& alloc, Ptr ptr, Args&&... args) {
        alloc.construct(ptr, std::forward<Args>(args)...);
    }

    template<typename Alloc, typename Ptr>
    void alloc_destroy(Alloc& alloc, Ptr ptr) {
        alloc.destroy(ptr);
    }

#else
    template<typename Alloc, typename Ptr, typename Arg>
    void alloc_construct(Alloc& alloc, Ptr ptr, const Arg& arg) {
        alloc.construct(ptr, arg);
    }

    template<typename Alloc, typename Ptr>
    void alloc_destroy(Alloc& alloc, Ptr ptr) {
        alloc.destroy(ptr);
    }
#endif


private:
    tipodato *datos_;    /// < Puntero al arreglo dinamico
    size_t tamano_;         /// < Cantidad actual de elementos
    size_t capacidad_;   /// < Capacidad total reservada
    Allocator alloc;    /// < Allocator

public:
    using value_type = tipodato;
    using reference = tipodato&;
    using const_reference = const tipodato&;
    using pointer = tipodato*;
    using const_pointer = const tipodato*;
    using iterator = tipodato*;
    using const_iterator = const tipodato*;

    //Constructores
#if __cplusplus<201103L
    /**
     * @brief Constructor para C++98 y C++03
     * Crea un vector vacio con capacidad y tamaño 0
     **/
    Vector() : datos_(NULL), tamano_(0), capacidad_(0) {}
#endif
#if __cplusplus>=201103L
    /**
     * @brief Constructor para C++98 y C++03
     * Crea un vector vacio con capacidad y tamaño 0.
     * Constructor para C++11+, usando nullptr en vez de NULL
     **/
    Vector() : datos_(nullptr), tamano_(0), capacidad_(0) {}

    /**
     * @brief Constructor para la initializer list
     *  @param lista
     **/
    Vector(std::initializer_list<tipodato> lista) {
        tamano_ = capacidad_ = lista.size();
        datos_ = alloc.allocate(capacidad_);
        size_t i = 0;
        for (auto &dato : lista) {
            alloc_construct(alloc,&datos_[i++], dato);
        }
    }
#endif

    /**
     * @brief Constructor que admite una capacidad y valor inicial
     * @param Capacidad
     * @param valor
     */
    explicit Vector(size_t Capacidad, const tipodato &valor = tipodato()) {
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


#if __cplusplus>=201103L
    //
    //  Move semantics (solo >=C++11)
    //

    /**
     * @brief Constructor para move semantics, admite otro vector como parametro
     * @param otro
     */
    Vector(Vector &&otro) noexcept {
        datos_ = otro.datos_;
        tamano_ = otro.tamano_;
        capacidad_ = otro.capacidad_;
        otro.datos_ = nullptr;
        otro.tamano_ = 0;
        otro.capacidad_ = 0;
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
            otro.datos_ = nullptr;
            otro.tamano_ = 0;
            otro.capacidad_ = 0;
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

#endif
#if __cplusplus<201103L
    /**
     * Constructor que permite la inclusion de otro vector como inicializador
     * @param otro
     */
    Vector(const Vector &otro) {
        tamano_ = otro.tamano_;
        capacidad_ = otro.capacidad_;
        datos_ = alloc.allocate(capacidad_);
        for (size_t i = 0; i < tamano_; i++) {
            alloc_construct(alloc, &datos_[i], otro.datos_[i]);
        }
    }
#endif
#if __cplusplus>=201103L
    /**
     * Constructor que permite la inclusion de otro vector como inicializador
     * @param otro
     */
    Vector(const Vector& otro) {
        datos_ = alloc.allocate(otro.capacidad_);
        tamano_ = otro.tamano_;
        capacidad_ = otro.capacidad_;
        for (size_t i = 0; i < tamano_; ++i) {
            alloc_construct(alloc, &datos_[i], otro.datos_[i]);
        }
    }
#endif

    //
    //  INICIO SECCION ITERADORES
    //  (usados para range based loops, algoritmos de ordenamiento y otras cosas)
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
#if __cplusplus < 202002L
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = tipodato;
        using difference_type   = std::ptrdiff_t;
        using pointer           = tipodato*;
        using reference         = tipodato&;
#else
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;
#endif
#if __cplusplus<202002L
        /**
         * @brief Constructor del iterador
         * @param p
         */
        explicit Iterator(tipodato *p) {
            ptr = p;
        }
        /**
         * @brief Constructor del iterador
         * @param otro
         *
         **/
        Iterator(const Iterator &otro) {
            ptr = otro.ptr;
        }

        /**
         * @brief Operador para asignar un iterador a otro
         * @param otro
         * @return
         */
        Iterator &operator=(const Iterator &otro) {
            if (this!=&otro) ptr = otro.ptr;
            return *this;
        }

        /**
         * @brief Operador de desrefenciación
         * @return
         */
        tipodato &operator*() {
            return *ptr;
        }

        /**
         * @brief Operador flecha para acceder a miembros del objeto apuntado
         * @return
         */
        tipodato *operator->() {
            return ptr;
        }

        //  Operaciones de incremento/decremento

        /**
         * @brief Operador de pre-incremento.
         * Avanza el iterador al siguiente elemento.
         * @return Referencia al iterador modificado.
         */
        Iterator &operator++() {
            ++ptr;
            return *this;
        }

        /**
         * @brief Operador de post-incremento.
         * Avanza el iterador al siguiente elemento, devolviendo una copia del estado previo.
         * @return Copia del iterador antes del incremento.
         */
        Iterator operator++(int) {
            Iterator aux = *this;
            ++ptr;
            return aux;
        }

        /**
        * @brief Operador de pre-decremento.
        * Retrocede el iterador al elemento anterior.
        * @return Referencia al iterador modificado.
        */
        Iterator &operator--() {
            --ptr;
            return *this;
        }

        /**
        * @brief Operador de post-decremento.
        * Retrocede el iterador al elemento anterior, devolviendo una copia del estado previo.
        * @return Copia del iterador antes del decremento.
        */
        Iterator operator--(int) {
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
        Iterator operator+(int n) const {
            return Iterator(ptr+n);
        }

        /**
        * @brief Operador de resta para retroceder el iterador.
        * @param n Número de posiciones a retroceder.
        * @return Nuevo iterador desplazado -n posiciones.
        */
        Iterator operator-(int n) const {
            return Iterator(ptr-n);
        }

        /**
         * @brief Operador de suma-igual.
         * Avanza el iterador n posiciones.
         * @param n Número de posiciones a avanzar.
         * @return Referencia al iterador modificado.
         */
        Iterator &operator+=(int n) {
            ptr+=n;
            return *this;
        }

        /**
         * @brief Operador de resta-igual.
         * Retrocede el iterador n posiciones.
         * @param n Número de posiciones a retroceder.
         * @return Referencia al iterador modificado.
         */
        Iterator &operator-=(int n) {
            ptr-=n;
            return *this;
        }

        /**
         * @brief Operador de diferencia entre iteradores.
         * @param o Otro iterador.
         * @return Número de posiciones de diferencia entre los iteradores.
         */
        difference_type operator-(const Iterator &o) const {
            return ptr - o.ptr;
        }

        /**
         * @brief Acceso por índice relativo desde la posición actual del iterador.
         * @param indice Índice relativo.
         * @return Referencia al elemento en la posición indicada.
         */
        tipodato &operator[](int indice) {
            return *(ptr+indice);
        }

        //  Comparaciones
        /**
         * @brief Operador de desigualdad.
         * @param o Otro iterador.
         * @return true si los iteradores apuntan a distintas posiciones.
         */
        bool operator!=(const Iterator &o) const {
            return ptr != o.ptr;
        }
        /**
         * @brief Operador menor que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición anterior.
         */
        bool operator<(const Iterator &o) const {
            return ptr < o.ptr;
        }
        /**
         * @brief Operador de igualdad.
         * @param o Otro iterador.
         * @return true si los iteradores apuntan a la misma posición.
         */
        bool operator==(const Iterator &o) const {
            return ptr == o.ptr;
        }
        /**
         * @brief Operador mayor que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición posterior.
         */
        bool operator>(const Iterator &o) const {
            return ptr > o.ptr;
        }
        /**
         * @brief Operador menor o igual que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición anterior o igual.
         */
        bool operator<=(const Iterator &o) const {
            return ptr <= o.ptr;
        }
        /**
         * @brief Operador mayor o igual que.
         * @param o Otro iterador.
         * @return true si el iterador actual apunta a una posición posterior o igual.
         */
        bool operator>=(const Iterator &o) const {
            return ptr >= o.ptr;
        }
    };
#else
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
#endif

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

#if __cplusplus>=202002L
    static_assert(std::input_iterator<Iterator>);

    friend constexpr Iterator operator+(std::ptrdiff_t n, const Iterator &it) {
        return it + n;
    }
#endif

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
#if __cplusplus < 202002L
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;
#else
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;
#endif

#if __cplusplus<202002L
        /**
        * @brief Constructor explícito desde puntero.
        * @param p Puntero al elemento.
        */
        explicit ReverseIterator(tipodato *p) {
            ptr=p;
        }

        /**
         * @brief Constructor por copia.
         * @param otro Otro ReverseIterator a copiar.
         */
        ReverseIterator(const ReverseIterator &otro) {
            ptr = otro.ptr;
        }

        /**
         * @brief Operador de asignación.
         * @param otro ReverseIterator a asignar.
         * @return Referencia al iterador resultante.
         */
        ReverseIterator &operator=(const ReverseIterator &otro) {
            if (this!=&otro) ptr = otro.ptr;
            return *this;
        }

        /**
         * @brief Retorna un iterador directo apuntando al siguiente elemento (inverso de reverse).
         * @return Un objeto de tipo Iterator.
         */
        Iterator base() const {
            return Iterator(ptr+1);
        }

        /**
         * @brief Operador de desreferenciación.
         * @return Referencia al elemento actual.
         */
        tipodato &operator*() {
            return *ptr;
        }
        /**
        * @brief Operador flecha para acceso a miembros.
        * @return Puntero al elemento actual.
        */
        tipodato &operator->() {
            return &(*ptr);
        }

        /**
         * @brief Conversión a Iterator directo.
         */
        explicit operator Iterator() const {
            return Iterator(ptr + 1);
        }

        //  Operador de incremento
        ReverseIterator &operator++() {
            --ptr; return *this;
        }
        //  Operador de incremento prefijo
        ReverseIterator operator++(int) {
            ReverseIterator aux = *this;
            --ptr;
            return aux;
        }
        //  Operador de decremento
        ReverseIterator &operator--() {
            ++ptr; return *this;
        }
        //  Operador de decremento prefijo
        ReverseIterator operator--(int) {
            ReverseIterator aux = *this;
            ++ptr;
            return aux;
        }

        //  Operador + que suma un entero al iterador
        ReverseIterator operator+(int n) const {
            return ReverseIterator(ptr-n);
        }
        //  Operador - que resta un entero al iterador
        ReverseIterator operator-(int n) const {
            return ReverseIterator(ptr+n);
        }
        //  Operador que agrega un valor entero al iterador
        ReverseIterator &operator+=(int n) {
            ptr-=n;
            return *this;
        }
        //  Operador que quita un valor entero del iterador
        ReverseIterator &operator-=(int n) {
            ptr+=n;
            return *this;
        }

        /**
        * @brief Diferencia entre dos iteradores.
        * @param o Otro ReverseIterator.
        * @return Número de elementos entre ellos.
        */
        int operator-(const ReverseIterator &o) const {
            return o.ptr - ptr;
        }

        /**
         * @brief Acceso aleatorio a elementos mediante índice.
         * @param indice Índice relativo desde el iterador actual.
         * @return Referencia al elemento correspondiente.
         */
        tipodato &operator[](int indice) {
            return *(ptr-indice);
        }

        //  Operadores booleanos
        //  Operador distinto de
        bool operator!=(const ReverseIterator &o) const {
            return ptr != o.ptr;
        }
        //  Operador igual que
        bool operator==(const ReverseIterator &o) const {
            return ptr == o.ptr;
        }
        //  Operador menor que
        bool operator<(const ReverseIterator &o) const {
            return ptr > o.ptr;
        }
        //  Operador mayor que
        bool operator>(const ReverseIterator &o) const {
            return ptr < o.ptr;
        }
        //  Operador menor o igual que
        bool operator<=(const ReverseIterator &o) const {
            return ptr >= o.ptr;
        }
        //  Operador mayor o igual que
        bool operator>=(const ReverseIterator &o) const {
            return ptr <= o.ptr;
        }
#else
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
#endif
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
#if __cplusplus>=202002L
    static_assert(std::input_iterator<ReverseIterator>);

    friend constexpr ReverseIterator operator+(std::ptrdiff_t n, const ReverseIterator &it) {
        return it + n;
    }
#endif

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
#if __cplusplus<202003L
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;
#else
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = const tipodato*;
        using reference = const tipodato&;
#endif
#if __cplusplus<202002L
        /**
         * @brief Constructor por defecto (C++03 o anterior).
         * @param p Puntero al dato constante.
         */
        #if __cplusplus<201103L
        explicit ConstIterator(const tipodato *p = NULL) {
            ptr = p;
        }
        #endif
        /**
         * @brief Constructor para nuevas versiones de C++ (C++11 o superior).
         * @param p Puntero al dato constante.
         */
        #if __cplusplus>=201103L
        explicit ConstIterator(const tipodato *p = nullptr) {
            ptr = p;
        }
        #endif

        /**
        * @brief Constructor copia.
        * @param o Otro iterador constante.
        */
        ConstIterator(const ConstIterator &o) {
            ptr = o.ptr;
        }

        /**
         * @brief Operador de asignación.
         * @param o Otro iterador constante.
         * @return Referencia a este iterador.
         */
        ConstIterator &operator=(const ConstIterator &o) {
            if (this!=&o) ptr = o.ptr;
            return *this;
        }

        /**
         * @brief Operador de desreferenciación.
         * @return Referencia constante al dato apuntado.
         */
        const tipodato &operator*() {
            return *ptr;
        }
        /**
         * @brief Operador flecha.
         * @return Puntero constante al dato apuntado.
         */
        const tipodato *operator->() {
            return ptr;
        }

        //  Operador de incremento
        ConstIterator &operator++() {
            ++ptr;
            return *this;
        }
        //  Operador de incremento postfijo
        ConstIterator operator++(int) {
            ConstIterator aux = *this;
            ++ptr;
            return aux;
        }
        //  Operador de decremento
        ConstIterator &operator--() {
            --ptr;
            return *this;
        }
        //  Operador de decremento postfijo
        ConstIterator operator--(int) {
            ConstIterator aux = *this;
            --ptr;
            return aux;
        }

        //  Operador + para sumar un valor entero al iterador
        ConstIterator operator+(int n) const {
            return ConstIterator(ptr+n);
        }
        //  Operador - para restar un valor entero al iterador
        ConstIterator operator-(int n) const {
            return ConstIterator(ptr-n);
        }
        //  Operador para agregar un valor entero al iterador
        ConstIterator &operator+=(int n) {
            ptr+=n;
            return *this;
        }
        //  Operador para quitar un valor entero del iterador
        ConstIterator &operator-=(int n) {
            ptr-=n;
            return *this;
        }

        /**
        * @brief Diferencia entre dos iteradores.
        * @param o Otro ReverseIterator.
        * @return Número de elementos entre ellos.
        */
        difference_type operator-(const ConstIterator &o) const {
            return ptr-o.ptr;
        }

        /**
         * @brief Operador de acceso por índice.
         * @param indice Desplazamiento desde el iterador actual.
         * @return Referencia constante al dato en la posición.
         */
        const tipodato &operator[](int indice) {
            return *(ptr+indice);
        }

        //  Operadores booleanos
        //  Operador igual que
        bool operator==(const ConstIterator &o) const {
            return ptr == o.ptr;
        }
        //  Operador distinto de
        bool operator!=(const ConstIterator &o) const {
            return ptr != o.ptr;
        }
        //  Operador menor que
        bool operator<(const ConstIterator &o) const {
            return ptr < o.ptr;
        }
        //  Operador mayor que
        bool operator>(const ConstIterator &o) const {
            return ptr > o.ptr;
        }
        //  Operador menor o igual que
        bool operator<=(const ConstIterator &o) const {
            return ptr >= o.ptr;
        }
        //  Operador mayor o igual que
        bool operator>=(const ConstIterator &o) const {
            return ptr <= o.ptr;
        }
#else
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
#endif
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
#if __cplusplus>=202002L
    static_assert(std::input_iterator<ConstIterator>);

    friend constexpr ConstIterator operator+(std::ptrdiff_t n, const ConstIterator &it) {
        return it + n;
    }
#endif

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
#if __cplusplus<202002L
        //  Cosas requeridas para que el iterador funcione correctamente
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = const tipodato*;
        using reference = const tipodato&;
#else
        using iterator_category = std::random_access_iterator_tag;
        using value_type = tipodato;
        using difference_type = std::ptrdiff_t;
        using pointer = tipodato*;
        using reference = tipodato&;
#endif
#if __cplusplus<202002L
        /**
         * @brief Constructor.
         * @param p Puntero al elemento actual (último de la colección).
         */
        explicit ConstReverseIterator(tipodato *p) {
            ptr=p;
        }

        /**
        * @brief Constructor copia.
        * @param otro Otro ConstReverseIterator.
        */
        ConstReverseIterator(const ConstReverseIterator &otro) {
            ptr = otro.ptr;
        }

        /**
         * @brief Operador de asignación.
         * @param otro Otro iterador a asignar.
         * @return Referencia a este iterador.
         */
        ConstReverseIterator &operator=(const ConstReverseIterator &otro) {
            if (this!=&otro) ptr = otro.ptr;
            return *this;
        }

        /**
         * @brief Retorna un ConstIterator correspondiente a la posición "base" (siguiente elemento).
         * @return ConstIterator apuntando a `ptr + 1`.
         */
        ConstIterator base() const {
            return ConstIterator(ptr+1);
        }

        /**
         * @brief Conversión implícita a Iterator.
         * @return Iterator equivalente apuntando a `ptr + 1`.
         */
        explicit operator Iterator() const {
            return Iterator(ptr + 1);
        }

        /**
         * @brief Operador de desreferenciación.
         * @return Referencia constante al elemento actual.
         */
        const tipodato &operator*() {
            return *ptr;
        }
        /**
        * @brief Operador flecha.
        * @return Puntero constante al elemento actual.
        */
        const tipodato &operator->() {
            return &(*ptr);
        }

        //  Operador de incremento
        ConstReverseIterator &operator++() {
            --ptr; return *this;
        }
        //  Operador de incremento postfijo
        ConstReverseIterator operator++(int) {
            ConstReverseIterator aux = *this;
            --ptr;
            return aux;
        }
        //  Operador de decremento
        ConstReverseIterator &operator--() {
            ++ptr; return *this;
        }
        //  Operador de decremento postfijo
        ConstReverseIterator operator--(int) {
            ConstReverseIterator aux = *this;
            ++ptr;
            return aux;
        }

        //  Operador para sumar un valor entero al iterador
        ConstReverseIterator operator+(int n) const {
            return ConstReverseIterator(ptr-n);
        }
        //  Operador para restar un valor entero del iterador
        ConstReverseIterator operator-(int n) const {
            return ConstReverseIterator(ptr+n);
        }
        //  Operador para agregar un valor entero al iterador
        ConstReverseIterator &operator+=(int n) {
            ptr-=n;
            return *this;
        }
        //  Operador para quitar un valor entero del iterador
        ConstReverseIterator &operator-=(int n) {
            ptr+=n;
            return *this;
        }

        //  Operador para restar un iterador de otro
        int operator-(const ConstReverseIterator &o) const {
            return ptr - o.ptr;
        }

        /**
         * @brief Operador de acceso por índice.
         * @param indice Desplazamiento desde el iterador actual (hacia atrás).
         * @return Referencia constante al elemento en la posición indicada.
         */
        const tipodato &operator[](int indice) {
            return *(ptr-indice);
        }

        //  Operadores booleanos
        //  Operador distinto de
        bool operator!=(const ConstReverseIterator &o) const {
            return ptr != o.ptr;
        }
        //  Operador igual a
        bool operator==(const ConstReverseIterator &o) const {
            return ptr == o.ptr;
        }
        //  Operador menor que
        bool operator<(const ConstReverseIterator &o) const {
            return ptr > o.ptr;
        }
        //  Operador mayor que
        bool operator>(const ConstReverseIterator &o) const {
            return ptr < o.ptr;
        }
        //  Operador menor o igual que
        bool operator<=(const ConstReverseIterator &o) const {
            return ptr >= o.ptr;
        }
        //  Operador mayor o igual que
        bool operator>=(const ConstReverseIterator &o) const {
            return ptr <= o.ptr;
        }
#else
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
#endif
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

#if __cplusplus>=202002L
    static_assert(std::input_iterator<ConstReverseIterator>);

    friend constexpr ConstReverseIterator operator+(std::ptrdiff_t n, const ConstReverseIterator &it) {
        return it + n;
    }
#endif

#if __cplusplus >= 202002L
    constexpr Iterator begin() noexcept { return Iterator(datos_); }
    constexpr Iterator end() noexcept { return Iterator(datos_ + tamano_); }

    constexpr ConstIterator cbegin() noexcept { return ConstIterator(datos_); }
    constexpr ConstIterator cend() noexcept { return ConstIterator(datos_ + tamano_); }

    constexpr ReverseIterator rbegin() noexcept { return ReverseIterator(datos_ + tamano_ - 1); }
    constexpr ReverseIterator rend() noexcept { return ReverseIterator(datos_ - 1); }

    constexpr ConstReverseIterator crbegin() noexcept { return ConstReverseIterator(datos_ + tamano_ - 1); }
    constexpr ConstReverseIterator crend() noexcept { return ConstReverseIterator(datos_ - 1); }
#endif

    //
    //  FIN DE LA SECCION DE ITERADORES
    //

#if __cplusplus<201103L
    /**
     * @brief Operador de asignación.
     *
     * Copia el contenido de otro vector en este vector.
     *
     * @param otro Vector a copiar.
     * @return Referencia a este vector.
     */
    Vector& operator=(const Vector &otro) {
        if (this != &otro) {
            auto *nuevo = new tipodato[otro.capacidad_];
            for (size_t i = 0; i < otro.tamano_; ++i) {
                nuevo[i] = otro.datos_[i];
            }
            delete[] datos_;
            datos_ = nuevo;
            tamano_ = otro.tamano_;
            capacidad_ = otro.capacidad_;
        }
        return *this;
    }
#endif

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

    //  Metodos de emplace y emplace back (SOLO COMPATIBLES CON C++11+)
#if __cplusplus>=201103L
    /**
     * @brief Construye un objeto en una posición específica del vector.
     *
     * Requiere C++11 o superior. Realiza construcción en el lugar.
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
            // Mover el elemento hacia la derecha usando move constructor o asignación
            alloc_construct(alloc, &datos_[i], std::move(datos_[i - 1]));
            alloc_destroy(alloc,&datos_[i - 1]);
        }

        alloc_construct(alloc, &datos_[indice], std::forward<Args>(args)...);

        ++tamano_;
    }

    /**
     * @brief Construye un objeto al final del vector.
     *
     * Requiere C++11 o superior. Realiza construcción en el lugar.
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
    }

#endif

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
            // destruir elementos sobrantes
            for (size_t i = nuevoTam; i < tamano_; ++i) {
                alloc_destroy(alloc,&datos_[i]);
            }
            tamano_ = nuevoTam;
            if (tamano_ < capacidad_ / 2) {
                reducirCapacidad();
            }
        } else {
            if (nuevoTam > capacidad_) {
                cambiarCapacidad(nuevoTam);
            }
            for (size_t i = tamano_; i < nuevoTam; i++) {
                alloc_construct(alloc, &datos_[i], dato);
            }
            tamano_ = nuevoTam;
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
    }
#if __cplusplus < 201103L
    /**
     * @brief Agrega un valor al final del vector.
     *
     * @param dato Valor a agregar.
     */
    void agregarFinal(const tipodato &dato) {
        if (tamano_ == capacidad_) {
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        }
        datos_[tamano_++] = dato;
    }
#endif
#if __cplusplus < 202003L
    /**
     * @brief Agrega un valor al final del vector.
     *
     * @param dato Valor a agregar.
     */
    void agregarFinal(tipodato&& dato) {
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
        if (tamano_ == capacidad_)
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        alloc_construct(alloc,&datos_[tamano_], dato);
        ++tamano_;
    }
#endif
#if __cplusplus >= 202002L
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
            std::vector<tipodato> temp_storage;
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

        return Iterator(datos_ + pos);
    }

    /**
     * @brief Agrega un valor al final del vector.
     *
     * @param dato Valor a agregar.
     */
    void agregarFinal(tipodato&& dato) {
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
        if (tamano_ == capacidad_)
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        alloc_construct(alloc,&datos_[tamano_], dato);
        ++tamano_;
    }
#endif


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

#if __cplusplus>=201103L
    /**
    * @brief Cambia la capacidad del vector.
    *
    * Si la nueva capacidad es menor o igual que la actual, no hace nada.
    *
    * @param nuevaCapacidad Nueva capacidad deseada.
    */
    void cambiarCapacidad(size_t nuevaCapacidad = 0) {
        if (nuevaCapacidad <= capacidad_) {
            return;
        }
        if (nuevaCapacidad == 0) {
            nuevaCapacidad = capacidad_ == 0 ? 1 : capacidad_ * 2;
        }

        tipodato* nuevo = alloc.allocate(nuevaCapacidad);
        size_t i = 0;

        try {
            for (; i < tamano_; ++i) {
                alloc_construct(alloc, &nuevo[i], std::move(datos_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                alloc_destroy(alloc,&nuevo[j]);
            }
            alloc.deallocate(nuevo, nuevaCapacidad);
            throw;
        }

        for (size_t j = 0; j < tamano_; ++j) {
            alloc_destroy(alloc,&datos_[j]);
        }
        if (datos_) {
            alloc.deallocate(datos_, capacidad_);
        }

        datos_ = nuevo;
        capacidad_ = nuevaCapacidad;
    }
#else
    void cambiarCapacidad(size_t nuevaCapacidad = 0) {
        if (nuevaCapacidad <= capacidad_) {
            return;
        }
        if (nuevaCapacidad == 0) {
            nuevaCapacidad = capacidad_ == 0 ? 1 : capacidad_ * 2;
        }

        tipodato* nuevo = alloc.allocate(nuevaCapacidad);
        size_t i = 0;
        try {
            for (; i < tamano_; ++i) {
                alloc_construct(alloc, &nuevo[i], datos_[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                alloc_destroy(alloc, &nuevo[j]);
            }
            alloc.deallocate(nuevo, nuevaCapacidad);
            throw;
        }

        for (size_t j = 0; j < tamano_; ++j) {
            alloc_destroy(alloc, &datos_[j]);
        }
        if (datos_) {
            alloc.deallocate(datos_, capacidad_);
        }

        datos_ = nuevo;
        capacidad_ = nuevaCapacidad;
    }

#endif
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

    //
    //  Insert y erase solo para C++98 y C++03
    //
#if __cplusplus<201103L
    /**
     * @brief Inserta un valor en el índice especificado.
     *
     * Solo disponible en C++98/C++03.
     *
     * @param indice Posición donde insertar el valor.
     * @param dato Valor a insertar.
     * @throws std::out_of_range si el índice está fuera de rango.
     */
    void insertar(size_t indice, const tipodato &dato) {
        if (indice > tamano_) {
            throw std::out_of_range("Indice fuera de rango");
        }
        if (tamano_ == capacidad_) {
            cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);
        }

        for (size_t i = tamano_; i > indice; i--) {
            alloc_construct(alloc, &datos_[i], datos_[i - 1]);
            alloc_destroy(alloc, &datos_[i - 1]);
        }

        alloc_construct(alloc, &datos_[indice], dato);

        ++tamano_;
    }

    /**
    * @brief Elimina el elemento apuntado por un iterador.
    *
    * Solo disponible en C++98/C++03.
    *
    * @param it Iterador al elemento a eliminar.
    * @return Iterador al siguiente elemento.
    * @throws std::out_of_range si el iterador está fuera de rango.
    */
    Iterator erase(Iterator it) {
        if (it.ptr < datos_ || it.ptr >= datos_ + tamano_) {
            throw std::out_of_range("Iterador fuera de rango");
        }

        size_t index = it.ptr - datos_;

        alloc_destroy(alloc, &datos_[index]);

        for (size_t i = index; i < tamano_ - 1; ++i) {
            alloc_construct(alloc, &datos_[i], datos_[i + 1]);
            alloc_destroy(alloc, &datos_[i + 1]);
        }

        --tamano_;

        return (index == tamano_) ? end() : Iterator(datos_ + index);
    }
#endif

    //
    //  Insert y erase para C++11 y C++14
    //

#if __cplusplus>=201103L && __cplusplus<201703L
    void insertar(size_t indice, const tipodato& dato) {
        if (indice > tamano_) throw std::out_of_range("Indice fuera de rango");
        if (tamano_ == capacidad_) cambiarCapacidad(capacidad_ == 0 ? 1 : capacidad_ * 2);

        if (std::is_trivially_copyable<tipodato>::value) {
            std::memmove(&datos_[indice + 1], &datos_[indice], sizeof(tipodato) * (tamano_ - indice));
            alloc_construct(&datos_[indice], dato);
        } else {
            for (size_t i = tamano_; i > indice; --i) {
                alloc_construct(&datos_[i], std::move(datos_[i - 1]));
                alloc_destroy(alloc, &datos_[i - 1]);
            }
            alloc_construct(&datos_[indice], dato);
        }

        ++tamano_;
    }

    Iterator erase(Iterator it) {
        if (it.ptr < datos_ || it.ptr >= datos_ + tamano_)
            throw std::out_of_range("Iterador fuera de rango");

        size_t idx = it.ptr - datos_;
        alloc_destroy(alloc, &datos_[idx]);

        if (std::is_trivially_copyable<tipodato>::value) {
            std::memmove(&datos_[idx], &datos_[idx + 1], sizeof(tipodato) * (tamano_ - idx - 1));
        } else {
            for (size_t i = idx; i < tamano_ - 1; ++i) {
                alloc_construct(&datos_[i], std::move(datos_[i + 1]));
                alloc_destroy(alloc, &datos_[i + 1]);
            }
        }
        --tamano_;
        return Iterator(datos_ + idx);
    }

    Iterator erase(Iterator first, Iterator last) {
        if (first == last) return first;
        size_t start = first - begin(), end = last - begin(), count = end - start;

        if (std::is_trivially_copyable<tipodato>::value) {
            std::memmove(&datos_[start], &datos_[end], sizeof(tipodato) * (tamano_ - end));
        } else {
            for (size_t i = 0; i < tamano_ - end; ++i) {
                alloc_construct(&datos_[start + i], std::move(datos_[end + i]));
                alloc_destroy(alloc, &datos_[end + i]);
            }
        }
        tamano_ -= count;
        return Iterator(datos_ + start);
    }
#endif

    //
    //  Insert y erase C++17 en adelante
    //

#if __cplusplus>=201703L
    /**
     * @brief Elimina el elemento apuntado por un iterador (C++17+).
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
     * @brief Elimina un rango de elementos (C++17+).
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
    * @brief Inserta un valor en el índice especificado (C++17+).
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
    }
#endif
#if __cplusplus>=201103L
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
#else
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
            alloc_construct(alloc, &datos_[i], datos_[i + 1]);
            alloc_destroy(alloc, &datos_[i + 1]);
        }
        --tamano_;
    }
#endif

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
#if __cplusplus<202002L
    /**
    * @brief Reduce la capacidad del vector a su tamaño actual.
    *
    * Libera memoria sobrante si existe.
    */
    void reducirCapacidad() {
        if (capacidad_ > tamano_) {
            auto *nuevo = new tipodato[tamano_];
            for (size_t i = 0; i < tamano_; i++) {
                nuevo[i] = datos_[i];
            }
            delete[] datos_;
            datos_ = nuevo;
            capacidad_ = tamano_;
        }
    }
#endif
#if __cplusplus >= 202002L
    /**
    * @brief Reduce la capacidad del vector a su tamaño actual.
    *
    * Libera memoria sobrante si existe.
    */
    void reducirCapacidad() {
        if (capacidad_ > tamano_) {
            auto* nuevo = new tipodato[tamano_];
            std::ranges::copy(std::ranges::subrange{begin(), begin() + tamano_}, nuevo);
            delete[] datos_;
            datos_ = nuevo;
            capacidad_ = tamano_;
        }
    }
#endif

    /**
    * @brief Intercambia el contenido de este vector con otro.
    * @param otro Vector con el cual se intercambiarán los datos.
    */
    void intercambiar(Vector &otro) {
        std::swap(datos_, otro.datos_);
        std::swap(tamano_, otro.tamano_);
        std::swap(capacidad_, otro.capacidad_);
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
    }

    /**
    * @brief Ordena el vector usando el algoritmo IntroSort (a través de std::sort).
    * Complejidad temporal: O(n log n)
    */
    void ordenar() {
        std::sort(begin(),end());
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
        for (size_t i = desde; i < hasta; ++i) {
            nuevo.agregarFinal(datos_[i]);
        }
        return nuevo;
    }

    /**
    * @brief Devuelve la capacidad restante disponible en el vector.
    * @return Número de elementos que se pueden insertar antes de redimensionar.
    */
    [[nodiscard]] size_t capacidadLibre() const {
        return capacidad_ - tamano_;
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
    }

    /**
    * @brief Elimina todos los elementos duplicados del vector.
    */
    void eliminarDuplicados() {
        ordenar();
        erase(std::unique(begin(), end()), end());
    }

    /**
    * @brief Cuenta cuántas veces aparece un dato en el vector.
    * @param dato Valor a contar.
    * @return Número de apariciones del valor.
    */
    size_t contar(const tipodato &dato) const {
        size_t contador = 0;                                //OMG TAYLOR SWIFT REFERENCE!!!!!!! 1989!!!!
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
    }

#if __cplusplus>=201103L
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
    }
#else
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
            // Use copy instead of move for C++98
            alloc_construct(alloc, &datos_[i + copia.tamano_ - 1], datos_[i - 1]);
            alloc_destroy(alloc, &datos_[i - 1]);
        }
        for (size_t i = 0; i < copia.tamano_; ++i) {
            alloc_construct(alloc, &datos_[indice + i], copia.datos_[i]);
        }
        tamano_ += copia.tamano_;
    }
#endif
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
    /**
    * @brief Swaps the contents with another vector.
    * @param other Vector with which to swap contents.
    */
    void swap(Vector &other) noexcept {
        intercambiar(other);
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
    * @brief Returns the maximum number of elements that the vector can theoretically hold.
    * @return Maximum size supported by the system.
    */
    size_t static max_size() {
        return std::numeric_limits<size_t>::max();
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
    * @brief Prints all elements in the vector to the console.
    */
    void display() const {
        mostrar();
    }
    /**
    * @brief Removes all duplicate values in the vector.
    */
    void removeDuplicates() {
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
    int swapIndices(size_t indexA, size_t indexB) {
        return intercambiarIndices(indexA, indexB);
    }
    /**
    * @brief Replaces all occurrences of a value with another.
    * @param oldValue Value to be replaced.
    * @param newValue Replacement value.
    * @return Number of elements replaced.
    */
    tipodato replaceAll(tipodato oldValue, tipodato newValue) {
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
    void bubbleSort() {
        ordenarBurbuja();
    }

#if __cplusplus >= 202002L
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
#endif

    ///@}

    //
    // [DEBUG]
    //

    // tipodato* raw() const {
    //     return datos;
    // }

};

#if __cplusplus >= 202002L
template<typename T, typename Alloc>
auto borrow_view(const Vector<T, Alloc>& vec) {
    return std::ranges::subrange(vec.begin(), vec.end());
}
#endif
#endif //CPPVECTOR_H