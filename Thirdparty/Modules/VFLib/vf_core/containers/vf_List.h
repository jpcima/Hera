/*============================================================================*/
/*
  VFLib: https://github.com/vinniefalco/VFLib

  Copyright (C) 2008 by Vinnie Falco <vinnie.falco@gmail.com>

  This library contains portions of other open source products covered by
  separate licenses. Please see the corresponding source files for specific
  terms.

  VFLib is provided under the terms of The MIT License (MIT):

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/
/*============================================================================*/

#ifndef VF_LIST_VFHEADER
#define VF_LIST_VFHEADER

struct ListDefaultTag;

/*============================================================================*/
/**
  Intrusive Containers

  # Introduction

  Intrusive containers are special containers that offer better performance
  and exception safety guarantees than non-intrusive containers (like the
  STL containers). They are useful building blocks for high performance
  concurrent systems or other purposes where allocations are restricted
  (such as the AudioIODeviceCallback object), because intrusive list
  operations do not allocate or free memory.

  While intrusive containers were and are widely used in C, they became more
  and more forgotten in C++ due to the presence of the standard containers
  which don't support intrusive techniques. VFLib not only reintroduces this
  technique to C++ for lists, it also encapsulates the implementation in a
  mostly compliant STL interface. Hence anyone familiar with standard
  containers can easily use them.

  # Interface

  The interface for intrusive elements in this library is unified for all
  containers. Unlike STL containers, objects placed into intrusive containers
  are not copied. Instead, a pointer to the object is stored. All
  responsibility for object lifetime is the responsibility of the caller;
  the intrusive container just keeps track of what is in it.

  Summary of intrusive container differences:

  - Holds pointers to existing objects instead of copies.

  - Does not allocate or free any objects.

  - Requires a element's class declaration to be modified.

  - Methods never throw exceptions when called with valid arguments.

  # Usage

  Like STL containers, intrusive containers are all template based, where the
  template argument specifies the type of object that the container will hold.
  These declarations specify a doubly linked list where each element points
  to a user defined class:

  @code

  class Object; // Forward declaration

  List <Object> list; // Doubly-linked list of Object

  @endcode

  Because intrusive containers allocate no memory, allowing objects to be
  placed inside requires a modification to their class declaration. Each
  intrusive container declares a nested class `Node` which elements must be
  derived from, using the Curiously Recurring Template Pattern (CRTP). We
  will continue to fully declare the Object type from the previous example
  to support emplacement into an intrusive container:

  @code

  class Object : public List <Object>::Node // Required for List
  {
  public:
    void performAction ();
  };

  @endcode

  Usage of a typedef eliminates redundant specification of the template
  arguments but requires a forward declaration. The following code is
  equivalent.

  @code

  class Object; // Forward declaration

  // Specify template parameters just once
  typedef List <Object> ListType;

  class Object : public ListType::Node
  {
    void performAction ();
  };

  ListType::Node list;

  @endcode

  With these declarations we may proceed to create our objects, add them to
  the list, and perform operations:

  @code

  // Create a few objects and put them in the list
  for (i = 0; i < 5; ++i)
    list.push_back (*new Object);

  // Call a method on each list
  for (ListType::iterator iter = list.begin(); iter != list.end (); ++iter)
    iter->performAction ();

  @endcode

  Unlike regular STL containers, an object derived from an intrusive container
  node cannot exist in more than one instance of that list at a time. This is
  because the bookkeeping information for maintaining the list is kept in
  the object rather than the list.

  To support objects existing in multiple containers, templates variations
  are instantiated by distinguishing them with an empty structure, called a
  tag. The object is derived from multiple instances of Node, where each
  instance specifies a unique tag. The tag is passed as the second template
  argument. When the second argument is unspecified, the default tag is used.

  This declaration example shows the usage of tags to allow an object to exist
  simultaneously in two separate lists:

  @code

  struct GlobalListTag { }; // list of all objects
  struct ActiveListTag { }; // subset of all objects that are active

  class Object : public List <Object, GlobalListTag>
               , public List <Object, ActiveListTag>
  {
  public:
    Object () : m_isActive (false)
    {
      // Add ourselves to the global list
      s_globalList.push_front (*this);
    }

    ~Object ()
    {
      deactivate ();
    }

    void becomeActive ()
    {
      // Add ourselves to the active list
      if (!m_isActive)
      {
        s_activeList.push_front (*this);
        m_isActive = true;
      }
    }

    void deactivate ()
    {
      if (m_isActive)
      {
        // Doesn't delete the object
        s_activeList.erase (s_activeList.iterator_to (this));

        m_isActive = false;
      }
    }

  private:
    bool m_isActive;

    static List <Object, GlobalListTag> s_globalList;
    static List <Object, ActiveListTag> s_activeList;
  }

  @endcode

  @defgroup intrusive intrusive
  @ingroup vf_core
*/

/*============================================================================*/
/**
  Intrusive doubly linked list.

  This intrusive List is a container similar in operation to std::list in the
  Standard Template Library (STL). Like all @ref intrusive containers, List
  requires you to first derive your class from List<>::Node:

  @code

  struct Object : List <Object>::Node
  {
    Object (int value) : m_value (value)
    {
    }

    int m_value;
  };

  @endcode

  Now we define the list, and add a couple of items.

  @code

  List <Object> list;

  list.push_back (* (new Object (1)));
  list.push_back (* (new Object (2)));

  @endcode

  For compatibility with the standard containers, push_back() expects a
  reference to the object. Unlike the standard container, however, push_back()
  places the actual object in the list and not a copy-constructed duplicate.

  Iterating over the list follows the same idiom as the STL:

  @code

  for (List <Object>::iterator iter = list.begin(); iter != list.end; ++iter)
    std::cout << iter->m_value;

  @endcode

  You can even use BOOST_FOREACH, or range based for loops:

  @code

  BOOST_FOREACH (Object& object, list)  // boost only
    std::cout << object.m_value;

  for (Object& object : list)           // C++11 only
    std::cout << object.m_value;

  @endcode

  Because List is mostly STL compliant, it can be passed into STL algorithms:
  e.g. `std::for_each()` or `std::find_first_of()`.

  In general, objects placed into a List should be dynamically allocated
  although this cannot be enforced at compile time. Since the caller provides
  the storage for the object, the caller is also responsible for deleting the
  object. An object still exists after being removed from a List, until the
  caller deletes it. This means an element can be moved from one List to
  another with practically no overhead.

  Unlike the standard containers, an object may only exist in one list at a
  time, unless special preparations are made. The Tag template parameter is
  used to distinguish between different list types for the same object,
  allowing the object to exist in more than one list simultaneously.

  For example, consider an actor system where a global list of actors is
  maintained, so that they can each be periodically receive processing
  time. We wish to also maintain a list of the subset of actors that require
  a domain-dependent update. To achieve this, we declare two tags, the
  associated list types, and the list element thusly:

  @code

  struct Actor;         // Forward declaration required

  struct ProcessTag { };
  struct UpdateTag { };

  typedef List <Actor, ProcessTag> ProcessList;
  typedef List <Actor, UpdateTag> UpdateList;

  // Derive from both node types so we can be in each list at once.
  //
  struct Actor : ProcessList::Node, UpdateList::Node
  {
    bool process ();    // returns true if we need an update
    void update ();
  };

  @endcode

  @tparam Element The base type of element which the list will store
                  pointers to.

  @tparam Tag An optional unique type name used to distinguish lists and nodes,
              when the object can exist in multiple lists simultaneously.

  @ingroup vf_core intrusive
*/
template <class Element, class Tag = ListDefaultTag>
class List
{
private:
    List (const List&) = delete;
    List& operator= (const List&) = delete;

public:
  typedef int size_type;

  typedef Element        value_type;
  typedef Element&       reference;
  typedef Element const& const_reference;
  typedef Element*       pointer;
  typedef Element const* const_pointer;

  class Node
  {
  private:
      Node (const Node&) = delete;
      Node& operator= (const Node&) = delete;

  public:
    Node () noexcept : m_next(nullptr), m_prev(nullptr)
    {
    }

  private:
    friend class List;
    Node* m_next;
    Node* m_prev;
  };

private:
  template <class ElemType, class NodeType>
  class iterator_base : public std::iterator <
    std::bidirectional_iterator_tag, int>
  {
  public:
    typedef ElemType value_type;
    typedef ElemType* pointer;
    typedef ElemType& reference;

    iterator_base (NodeType* node = nullptr) noexcept : m_node (node)
    {
    }

    template <class OtherElemType, class OtherNodeType>
    iterator_base (iterator_base <OtherElemType, OtherNodeType> const& other) noexcept
      : m_node (other.m_node)
    {
    }

    template <class OtherElemType, class OtherNodeType>
    iterator_base& operator= (iterator_base <OtherElemType, OtherNodeType> const& other) noexcept
    {
      m_node = other.m_node;
      return *this;
    }

    template <class OtherElemType, class OtherNodeType>
    bool operator == (iterator_base <OtherElemType, OtherNodeType> const& other) const noexcept
    {
      return m_node == other.m_node;
    }

    template <class OtherElemType, class OtherNodeType>
    bool operator != (iterator_base <OtherElemType, OtherNodeType> const& other) const noexcept
    {
      return ! this->operator== (other);
    }

    reference operator* () const noexcept
    {
      return dereference ();
    }

    pointer operator-> () const noexcept
    {
      return &dereference ();
    }

    iterator_base& operator++ () noexcept
    {
      increment ();
      return *this;
    }

    iterator_base operator++ (int) noexcept
    {
      iterator_base result (*this);
      increment ();
      return result;
    }

    iterator_base& operator-- () noexcept
    {
      decrement ();
      return *this;
    }

    iterator_base operator-- (int) noexcept
    {
      iterator_base result (*this);
      decrement ();
      return result;
    }

  private:
    friend class List;

    NodeType* get_node () noexcept
    {
      return m_node;
    }

    NodeType const* get_node () const noexcept
    {
      return m_node;
    }

    reference dereference () const noexcept
    {
      return *static_cast <ElemType*> (m_node);
    }

    bool equal (NodeType *const *node) const noexcept
    {
      return m_node == node;
    }

    void increment () noexcept
    {
      jassert (m_node->m_next);
      m_node = m_node->m_next;
    }

    void decrement () noexcept
    {
      jassert (m_node->m_prev && m_node->m_prev->m_prev != 0);
      m_node = m_node->m_prev;
    }

  private:
    NodeType* m_node;
  };

public:
  /** A read/write List iterator. */
  typedef iterator_base <Element, Node> iterator;

  /** A read-only List iterator. */
  typedef iterator_base <Element const, Node const> const_iterator;

public:
  /** Create an empty list. */
  List () noexcept : m_size (0)
  {
    m_head.m_prev = nullptr; // identifies the head
    m_tail.m_next = nullptr; // identifies the tail
    clear ();
  }

  /** Returns the number of elements in the list

      @return The number of elements in the list.
  */
  size_type size () const noexcept
  {
    return m_size;
  }

  /** Obtain a reference to the first element.

      @invariant The list may not be empty.

      @return A reference to the first element.
  */
  reference front () noexcept
  {
    jassert (!empty ());
    return element_from (m_head.m_next);
  }

  /** Obtain a const reference to the first element.

      @invariant The list may not be empty.

      @return A const reference to the first element.
  */
  const_reference front () const noexcept
  {
    jassert (!empty ());
    return element_from (m_head.m_next);
  }

  /** Obtain a reference to the last element.

      @invariant The list may not be empty.

      @return A reference to the last element.
  */
  reference back () noexcept
  {
    jassert (!empty ());
    return element_from (m_tail.m_prev);
  }

  /** Obtain a const reference to the last element.

      @invariant The list may not be empty.

      @return A const reference to the last element.
  */
  const_reference back () const noexcept
  {
    jassert (!empty ());
    return element_from (m_tail.m_prev);
  }

  /** Obtain an iterator to the beginning of the list.

      @return An iterator pointing to the beginning of the list.
  */
  iterator begin () noexcept
  {
    return iterator (m_head.m_next);
  }

  /** Obtain a const iterator to the beginning of the list.

      @return A const iterator pointing to the beginning of the list.
  */
  const_iterator begin () const noexcept
  {
    return const_iterator (m_head.m_next);
  }

  /** Obtain a const iterator to the beginning of the list.

      @return A const iterator pointing to the beginning of the list.
  */
  const_iterator cbegin () const noexcept
  {
    return const_iterator (m_head.m_next);
  }

  /** Obtain a iterator to the end of the list.

      @return An iterator pointing to the end of the list.
  */
  iterator end () noexcept
  {
    return iterator (&m_tail);
  }

  /** Obtain a const iterator to the end of the list.

      @return A constiterator pointing to the end of the list.
  */
  const_iterator end () const noexcept
  {
    return const_iterator (&m_tail);
  }

  /** Obtain a const iterator to the end of the list.

      @return A constiterator pointing to the end of the list.
  */
  const_iterator cend () const noexcept
  {
    return const_iterator (&m_tail);
  }

  /** Determine if the list is empty.

      @return `true` if the list is empty.
  */
  bool empty () const noexcept
  {
    return m_head.m_next == &m_tail;
  }

  /** Clear the list.

      @note This does not free the elements.
  */
  void clear () noexcept
  {
    m_head.m_next = &m_tail;
    m_tail.m_prev = &m_head;
    m_size = 0;
  }

  /** Insert an element.

      @invariant The element must not already be in the list.

      @param pos The location to insert after.

      @param elem The element to insert.

      @return An iterator pointing to the newly inserted element.
  */
  iterator insert (iterator pos, Element& elem) noexcept
  {
    Node* node = node_from (elem);
    node->m_next = pos.get_node ();
    node->m_prev = node->m_next->m_prev;
    node->m_next->m_prev = node;
    node->m_prev->m_next = node;
    ++m_size;
    return iterator (node);
  }

  /** Insert another list into this one.

      The other list is cleared.

      @param pos The location to insert after.

      @param other The list to insert.
  */

  void insert (iterator pos, List& other) noexcept
  {
    if (!other.empty ())
    {
      Node* before = pos.get_node ();
      other.m_head.m_next->m_prev = before->m_prev;
      before->m_prev->m_next = other.m_head.m_next;
      other.m_tail.m_prev->m_next = before;
      before->m_prev = other.m_tail.m_prev;
      m_size += other.m_size;
      other.clear ();
    }
  }

  /** Remove an element.

      @invariant The element must exist in the list.

      @param pos An iterator pointing to the element to remove.

      @return An iterator pointing to the next element after the one removed.
  */
  iterator erase (iterator pos) noexcept
  {
    Node* node = pos.get_node ();
    ++pos;
    node->m_next->m_prev = node->m_prev;
    node->m_prev->m_next = node->m_next;
    --m_size;
    return pos;
  }

  /** Insert an element at the beginning of the list.

      @invariant The element must not exist in the list.

      @param elem The element to insert.
  */
  void push_front (Element& elem) noexcept
  {
    insert (begin(), elem);
  }

  /** Remove the element at the beginning of the list.

      @invariant The list must not be empty.
  */
  void pop_front () noexcept
  {
    jassert (!empty ());
    erase (begin ());
  }

  /** Append an element at the end of the list.

      @invariant The element must not exist in the list.

      @param elem The element to append.
  */
  void push_back (Element& elem) noexcept
  {
    insert (end(), elem);
  }

  /** Remove the element at the end of the list.

      @invariant The list must not be empty.
  */
  void pop_back () noexcept
  {
    jassert (!empty ());
    erase (--end ());
  }

  /** Swap contents with another list.
  */
  void swap (List& other) noexcept
  {
    List temp;
    temp.append (other);
    other.append (*this);
    append (temp);
  }

  /** Insert another list at the beginning of this list.

      The other list is cleared.

      @param list The other list to insert.
  */
  void prepend (List& list) noexcept
  {
    insert (begin (), list);
  }

  /** Append another list at the end of this list.

      The other list is cleared.

      @param list the other list to append.
  */
  void append (List& list) noexcept
  {
    insert (end(), list);
  }

  /** Obtain an iterator from an element.

      @invariant The element must exist in the list.

      @param elem The element to obtain an iterator for.

      @return An iterator to the element.
  */
  iterator iterator_to (Element& elem) const noexcept
  {
    return iterator (static_cast <Node*> (&elem));
  }

  /** Obtain a const iterator from an element.

      @invariant The element must exist in the list.

      @param elem The element to obtain an iterator for.

      @return A const iterator to the element.
  */
  const_iterator const_iterator_to (Element const& elem) const noexcept
  {
    return const_iterator (static_cast <Node const*> (&elem));
  }

private:
  inline reference element_from (Node* node) noexcept
  {
    return *(static_cast <pointer> (node));
  }

  inline const_reference element_from (Node const* node) const noexcept
  {
    return *(static_cast <const_pointer> (node));
  }

  inline Node* node_from (Element& elem) noexcept
  {
    return static_cast <Node*> (&elem);
  }

  inline Node const* node_from (Element const& elem) const noexcept
  {
    return static_cast <Node const*> (&elem);
  }

private:
  size_type m_size;
  Node m_head;
  Node m_tail;
};

/**
  Default tag for List.

  @ingroup vf_core intrusive
*/
struct ListDefaultTag { };

#endif
