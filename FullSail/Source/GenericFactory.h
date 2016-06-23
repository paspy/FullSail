namespace Factory {
    template <class TKey, class TType>
    class GenericFactory {
        // This class implements a generic factory that can be used to create any type with any number of arguments.  Template arguments:
        // TKey - the first class template type is provided so that the object type name can be defined as a string, int, double, etc.
        // TType - base class of derived types to be created.
        // 
        // The idea behind the class factory method is to generate a specific concrete type of object based upon a key, which in this case is the TKey template parameter.
        // For example, the string "dog" could be used to create a dog object of an animal class.  To accomplish this, a Register method is defined which must be called to
        // map the string to the static function used to create the object.  For example:
        //
        // GenericFactory<string, Animal> GenFactAnimal; // Creates a class factory that can create derived objects of the Animal base class, and whose TKey type is string.
        // GenFactAnmal.Register("dog", &Dog::CreateObject); // Maps the string dog to the Dog CreateObject method, which must be declared as static.
        // Once one or more derived classes have been mapped, then the CreateObj method can be called to create an object specified by the type name.  For example:
        // unique_ptr<Animal> pDog(GenFactAnimal.CreateObj("dog")); // Create the dog object and wrap it inside of a unique_ptr.
        //
        typedef TType* ( *CreateObjFn )( vector<void*> &args ); // Defines the CreateObjFn function pointer that points to the object creation function.
        typedef tr1::unordered_map<TKey, CreateObjFn> FactoryMap; // Hash table to map the key to the function used to create the object.
    public:
        void Register( const TKey &keyName, CreateObjFn pCreateFn ) {
            // Store the function pointer to create this object in the hash table.
            FactMap[keyName] = pCreateFn;
        }

        TType* CreateObj( const TKey &keyName, vector<void*> &args ) {
            // This method looks for the name in the hash map.  If it is not found, then an exception is thrown.
            // If it is found, then it creates the specified object and returns a pointer to it.
            //
            FactoryMap::iterator It = FactMap.find( keyName );
            if ( It != FactMap.end() ) {
                return It->second( args );
            }
            throw "GenericFactory::CreateObj: key was not found in hashtable.  Did you forget to register it?";
        }
    private:
        FactoryMap FactMap;
    };
}