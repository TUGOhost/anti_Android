/**
    \file JavaArrays.hpp
    \brief Creation of java arrays.
    \author Denis Sorokin
    \date 22.02.2016
*/

/**
* Cheat sheet:
*
* @code{.cpp}
*
* // Define custom java class:
* JH_JAVA_CUSTOM_CLASS(JavaExample, "com/some/path/Example");
*
* // Create int array:
* std::vector<int> v({10, 15, 20});
* jintArray intArray = jh::JavaArrayBuilder<int>()
*     .add(5)                   // value
*     .add({25, 30})            // initializer_list
*     .add(v.begin(), v.end())  // iterators
*     .build());
*
* // Create jstring array:
* jobjectArray stringArray = jh::JavaArrayBuilder<jstring>()
*     .add(jh::createJString("someString"))
*     .build();
*
* // Create custom java object array:
* jobjectArray exampleArray = jh::JavaArrayBuilder<JavaExample>()
*     .add(jh::createNewObject<JavaExample>())
*     .build();
*
* // Parse java int array:
* auto stdIntVector = jh::jarrayToVector<jintArray>(intArray);
* for (auto i : stdIntVector) {
*     cout << i << endl;
* }
*
* // Passing java array to java code:
* jh::callMethod<void, jh::JavaArray<JavaExample>>(exampleObject, "someMethod", exampleArray);
*
* // Getting java array from java code:
* jobjectArray someArray = jh::callStaticMethod<JavaExample, JavaArray<jstring>>("otherMethod");
*
* @endcode
*/

#ifndef JH_JAVA_ARRAYS_HPP
#define JH_JAVA_ARRAYS_HPP

#include <jni.h>
#include <vector>
#include "../core/ToJavaType.hpp"
#include "../core/JNIEnvironment.hpp"
#include "../arrays/ArrayAllocator.hpp"
#include "../arrays/ArrayGetter.hpp"
#include "../arrays/ArraySetter.hpp"

namespace jh
{
    /**
    * Transforms and returns the java array data as an std::vector object.
    *
    * @param JavaArrayType Type of jarray. Can be jintArray, jobjectArray, etc.
    * @param array The pointer to the specified array type instance.
    *
    * @return std::vector with the cope of all elements from the java array.
    */
    template<class JavaArrayType>
    std::vector<typename ToJavaType<JavaArrayType>::ElementType> jarrayToVector(JavaArrayType array)
    {
        auto env = getCurrentJNIEnvironment();
        return std::move(JavaArrayGetter<JavaArrayType>::get(env, array));
    }

    /**
    * Template prototype for java array builder.
    *
    * @param JavaArrayType Type of java array. Can be jfloatArray, jobjectArray, etc.
    * @param ElementType Type of the elements inside this array.
    */
    template<class JavaArrayType, class ElementType>
    class JavaBaseArrayBuilder
    {
        /**
        * Type of all elements that can be added to this builder.
        */
        using ArgumentType = typename ToJavaType<ElementType>::CallReturnType;

    public:
        /**
        * Default constructor. Array has no elements.
        */
        JavaBaseArrayBuilder() = default;

        /**
        * Almost the same as the default constructor, but it allocates the space for 'size' elements.
        *
        * @param size Number of elements in the future array.
        */
        JavaBaseArrayBuilder(std::size_t size) : m_elements(size) { };

        /**
        * Adds one element to the array.
        *
        * @param value Value of the new element.
        */
        JavaBaseArrayBuilder<JavaArrayType, ElementType>& add(ArgumentType value)
        {
            m_elements.push_back(value);
            return *this;
        }

        /**
        * Adds several elements to the array from the initializer list.
        *
        * @param container Initializer list with several values.
        */
        JavaBaseArrayBuilder<JavaArrayType, ElementType>& add(std::initializer_list<ArgumentType> container)
        {
            for (auto element : container)
                m_elements.push_back(element);

            return *this;
        }

        /**
        * Adds several elements to the array from one iterator to another.
        *
        * @param b Iterator to the first element.
        * @param e Iterator to the last element.
        * @param p Hacky way to pass only correct iterators to this method.
        */
        template <class Iterator>
        JavaBaseArrayBuilder<JavaArrayType, ElementType>& add(Iterator b, Iterator e, typename Iterator::iterator_category* p = 0)
        {
            for (auto it = b; it != e; ++it)
                m_elements.push_back(*it);

            return *this;
        }

        /**
        * Constructs new java array and returns it.
        *
        * @return New java array with the stored elements.
        */
        JavaArrayType build()
        {
            JNIEnv* env = getCurrentJNIEnvironment();
            JavaArrayType array = JavaArrayAllocator<JavaArrayType, ElementType>::create(env, m_elements.size());
            JavaArraySetter<JavaArrayType>::set(env, array, m_elements.size(), &m_elements[0]);
            return array;
        }

    protected:
        /**
        * Container for elements that should be in the created hava array.
        */
        std::vector<ArgumentType> m_elements;
    };

    /**
    * Java array builder for the java objects (default & custom).
    */
    template<class ElementType>
    class JavaArrayBuilder : public JavaBaseArrayBuilder<jobjectArray, ElementType> { };

    /**
    * Java array builder for the boolean array.
    */
    template<>
    class JavaArrayBuilder<bool> : public JavaBaseArrayBuilder<jbooleanArray, bool> { };

    /**
    * Java array builder for the int array.
    */
    template<>
    class JavaArrayBuilder<int> : public JavaBaseArrayBuilder<jintArray, int> { };

    /**
    * Java array builder for the long array.
    */
    template<>
    class JavaArrayBuilder<long> : public JavaBaseArrayBuilder<jlongArray, long> { };

    /**
    * Java array builder for the float array.
    */
    template<>
    class JavaArrayBuilder<float> : public JavaBaseArrayBuilder<jfloatArray, float> { };

    /**
    * Java array builder for the double array.
    */
    template<>
    class JavaArrayBuilder<double> : public JavaBaseArrayBuilder<jdoubleArray, double> { };
}

#endif
