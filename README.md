> [!NOTE]
> This is an educational/personal project, don't expect it to be as optimized or functional as std::vector

# cppvector

A custom dynamic vector implementation in C++, with full support for STL algorithms, iterators and bilingual method names in **English** and **Spanish**.
The library includes full support for all C++ versions (note that some features may not be available for older version).

## Features

- Dynamic resizing of the array
- Compatibility with STL algorithms (such as std::sort, std::find, etc)
- Bilingual support for most methods
- Arithmetic operators
- Iterators
- Move semantics
- Support for higher dimension arrays

## What are the methods included in the library?

### Element access

| std::sort   | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| at()        | at()                | en()                |
| back()      | back()              | atras()             |
| data()      | data()              | data()              |
| front()     | front()             | frente()            |

### Iterators
| std::sort   | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| begin()     | begin()             | begin()             |
| cbegin()    | cbegin()            | cbegin()            |
| cend()      | cend()              | cend()              |
| crbegin()   | crbegin()           | crbegin()           |
| crend()     | crend()             | crend()             |
| rend()      | rend()              | rend()              |
| end()       | end()               | end()               |

### Capacity
| std::sort   | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| capacity()  | capacity()          | obtenerCapacidad()  |
| empty()     | empty()             | vacio()             |
| max_size()  | max_size()          | max_size()          |
| reserve()   | reserve()           | reservar()          | 
| shrink_to_fit() | shrink_to_fit() | reducirCapacidad()  |
| size()      | size()              | obtenerTamano()     |

### Modifiers
| std::sort   | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| clear()     | clear()             | limpiar()           |
| emplace()   | emplace()           | emplace()           |
| emplace_back() | emplace_back()   | emplace_back()      |
| erase()     | erase()             | erase()             |
| insert()    | insert()            | insertar()          |
| pop_back()  | pop_back()          | eliminarFinal()     |
| push_back() | push_back()         | agregarFinal()      |
| resize()    | resize()            | redimensionar()     |
| swap()      | swap()              | intercambiar()      |

### Custom methods
| cppvector (English) | cppvector (Spanish) |
|---------------------|---------------------|
| ordenar()           | ordenar()           |
| ordenarBurbuja()    | ordenarBurbuja()    |
| mostrar()           | mostrar()           |
| eliminarDuplicados() | eliminarDuplicados() |
| invertir()          | invertir()          |
| intercambiarIndices() | intercambiarIndices() |
| reemplazar()        | reemplazar()        |
| subvector()         | subvector()         |

#### Custom methods explaination

- ordenar() uses std::sort to sort the vector in ascending order
- ordenarBurbuja() uses bubble sort to sort the vector in ascending order (just for fun, please don't use it in large datasets!)
- mostrar() prints all elements stored in the vector to console
- eliminarDuplicados() deletes all duplicate values in the vector
- invertir() inverts the order of elements in the vector
- intercambiarIndices() swaps position numbers of two provided elements
- reemplazar() replaces all apparitions of an element in the vector
- subvector() creates a subvector in a specified range of elements

### TODO
- Add allocator support
- Add append_range() support (C++23+)
- Add insert_range() support (C++23+)
- Translate more methods to Spanish



