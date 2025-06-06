> [!NOTE]
> This is an educational/personal project — don't expect it to be as optimized or functional as std::vector

# cppvector

A custom dynamic vector implementation in C++, with full support for STL algorithms, iterators and bilingual method names in **English** and **Spanish**.
The library supports all C++ versions (note that some features may not be available in older versions).

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

| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| at()        | at()                | en()                |
| back()      | back()              | atras()             |
| data()      | data()              | data()              |
| front()     | front()             | frente()            |

### Iterators
| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| begin()     | begin()             | begin()             |
| cbegin()    | cbegin()            | cbegin()            |
| cend()      | cend()              | cend()              |
| crbegin()   | crbegin()           | crbegin()           |
| crend()     | crend()             | crend()             |
| rend()      | rend()              | rend()              |
| end()       | end()               | end()               |

### Capacity
| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| capacity()  | capacity()          | obtenerCapacidad()  |
| empty()     | empty()             | vacio()             |
| max_size()  | max_size()          | max_size()          |
| reserve()   | reserve()           | reservar()          | 
| shrink_to_fit() | shrink_to_fit() | reducirCapacidad()  |
| size()      | size()              | obtenerTamano()     |

### Modifiers
| STL Method  | cppvector (English) | cppvector (Spanish) |
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
| sort()              | ordenar()           |
| bubbleSort()        | ordenarBurbuja()    |
| display()           | mostrar()           |
| removeDuplicates()  | eliminarDuplicados() |
| reverse ()          | invertir()          |
| swapIndices()       | intercambiarIndices() |
| replaceAll()        | reemplazar()        |
| slice()             | subvector()         |

#### Custom methods explanation

- ordenar() uses std::sort to sort the vector in ascending order.
- ordenarBurbuja() uses bubble sort to sort the vector in ascending order (just for fun — avoid for large datasets!).
- mostrar() prints all elements stored in the vector to console.
- eliminarDuplicados() deletes all duplicate values in the vector.
- invertir() inverts the order of elements in the vector.
- intercambiarIndices() swaps indices of two provided elements.
- reemplazar() replaces all occurrences of an element in the vector.
- subvector() creates a subvector in a specified range of elements.

### TODO
- Add allocator support
- Add append_range() support (C++23+)
- Add insert_range() support (C++23+)
- ~~Translate more methods to Spanish~~

### Usage example
```c++
#include "cppvector.h"

int main() {
    Vector<int> v;
    v.push_back(10); // You can also use v.agregarFinal(10);
    v.push_back(20); 
    v.mostrar(); // Should print: 10 20

    return 0;
}
```

