#include <iostream>         // std::cout, std::endl
#include <string>           // std::string
#include <memory>           // std::shared_ptr

#include "factory.hpp"      // Factory
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);
static int TestMultipleTypes(void);
static int TestExceptions(void);
static int TestParameters(void);

/***************************** Test Classes **********************************/

// Base class for our tests
class Animal 
{
public:
    virtual ~Animal() = default;
    virtual string GetSound() const = 0;
    virtual string GetType() const = 0;
};

// Derived class 1
class Dog : public Animal 
{
public:
    Dog(const string& name) : m_name(name) {}
    
    string GetSound() const override { return "Woof!"; }
    string GetType() const override { return "Dog"; }
    string GetName() const { return m_name; }
    
private:
    string m_name;
};

// Derived class 2
class Cat : public Animal 
{
public:
    Cat(const string& name) : m_name(name) {}
    
    string GetSound() const override { return "Meow!"; }
    string GetType() const override { return "Cat"; }
    string GetName() const { return m_name; }
    
private:
    string m_name;
};

// Derived class 3 with multiple constructor parameters
class Bird : public Animal 
{
public:
    Bird(const string& name, const string& species) : 
        m_name(name), m_species(species) {}
    
    string GetSound() const override { return "Tweet!"; }
    string GetType() const override { return "Bird"; }
    string GetName() const { return m_name; }
    string GetSpecies() const { return m_species; }
    
private:
    string m_name;
    string m_species;
};

/******************************* Main ****************************************/

int main()
{
    int status = 0;
    
    status += Test();
 
    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") : 
                    (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");
    
    return (0);
}

/***************************** Static Functions ******************************/

static int Test(void)
{
    int status = 0;
   
    cout << "\n\033[1m----------- Testing Factory -----------\033[0m\n";
    
    status += TestBasic();
    status += TestMultipleTypes();
    status += TestExceptions();
    status += TestParameters();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
    
    // Create a factory for Animal objects identified by string keys
    Factory<Animal, string, string> animalFactory;
    
    // Register creation functions for different animal types
    animalFactory.Add("dog", [](const string& name) -> shared_ptr<Animal> 
    {
        return make_shared<Dog>(name);
    });
    
    // Create an animal using the factory
    shared_ptr<Animal> animal = animalFactory.Create("dog", "Buddy");
    
    // Verify animal was created correctly
    status += CheckValue(animal != nullptr, true, "Animal creation", __LINE__);
    status += CheckValue(animal->GetType() == "Dog", true, "Animal type", __LINE__);
    status += CheckValue(animal->GetSound() == "Woof!", true, "Animal sound", __LINE__);
    
    // Check that the name was passed correctly
    shared_ptr<Dog> dog = dynamic_pointer_cast<Dog>(animal);
    status += CheckValue(dog != nullptr, true, "Dynamic cast to Dog", __LINE__);
    status += CheckValue(dog->GetName() == "Buddy", true, "Dog name", __LINE__);
    
    return (status);
}

static int TestMultipleTypes(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting multiple registered types:\033[0m\n";
    
    // Create a factory for Animal objects identified by string keys
    Factory<Animal, string, string> animalFactory;
    
    // Register creation functions for different animal types
    animalFactory.Add("dog", [](const string& name) -> shared_ptr<Animal> 
    {
        return make_shared<Dog>(name);
    });
    
    animalFactory.Add("cat", [](const string& name) -> shared_ptr<Animal> 
    {
        return make_shared<Cat>(name);
    });
    
    // Create animals using the factory
    shared_ptr<Animal> dog = animalFactory.Create("dog", "Buddy");
    shared_ptr<Animal> cat = animalFactory.Create("cat", "Whiskers");
    
    // Verify animals were created correctly
    status += CheckValue(dog->GetType() == "Dog", true, "Dog type", __LINE__);
    status += CheckValue(cat->GetType() == "Cat", true, "Cat type", __LINE__);
    
    status += CheckValue(dog->GetSound() == "Woof!", true, "Dog sound", __LINE__);
    status += CheckValue(cat->GetSound() == "Meow!", true, "Cat sound", __LINE__);
    
    return (status);
}

static int TestExceptions(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting exception handling:\033[0m\n";
    
    // Create a factory for Animal objects identified by string keys
    Factory<Animal, string, string> animalFactory;
    
    // Register a creation function
    animalFactory.Add("dog", [](const string& name) -> shared_ptr<Animal> 
    {
        return make_shared<Dog>(name);
    });
    
    // Test adding duplicate key
    bool exceptionThrown = false;
    try 
    {
        animalFactory.Add("dog", [](const string& name) -> shared_ptr<Animal> 
        {
            return make_shared<Dog>(name);
        });
    } 
    catch (const out_of_range& e) 
    {
        exceptionThrown = true;
    }

    status += CheckValue(exceptionThrown, false, "Exception for duplicate key", __LINE__);
    
    // Test creating with non-existent key
    exceptionThrown = false;
    try 
    {
        shared_ptr<Animal> animal = animalFactory.Create("elephant", "Dumbo");
    } 
    catch (const out_of_range& e) 
    {
        exceptionThrown = true;
    }

    status += CheckValue(exceptionThrown, true, "Exception for non-existent key", __LINE__);
    
    return (status);
}

static int TestParameters(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting with multiple parameters:\033[0m\n";
    
    // Create a factory for Animal objects identified by string keys
    // This one takes two parameters: name and species
    Factory<Animal, string, string, string> animalFactory;
    
    // Register creation function for bird that takes two parameters
    animalFactory.Add("bird", [](const string& name, const string& species) -> shared_ptr<Animal> 
    {
        return make_shared<Bird>(name, species);
    });
    
    // Create a bird using the factory with two parameters
    shared_ptr<Animal> animal = animalFactory.Create("bird", "Polly", "Parrot");
    
    // Verify bird was created correctly
    status += CheckValue(animal != nullptr, true, "Bird creation", __LINE__);
    status += CheckValue(animal->GetType() == "Bird", true, "Bird type", __LINE__);
    status += CheckValue(animal->GetSound() == "Tweet!", true, "Bird sound", __LINE__);
    
    // Check that both parameters were passed correctly
    shared_ptr<Bird> bird = dynamic_pointer_cast<Bird>(animal);
    status += CheckValue(bird != nullptr, true, "Dynamic cast to Bird", __LINE__);
    status += CheckValue(bird->GetName() == "Polly", true, "Bird name", __LINE__);
    status += CheckValue(bird->GetSpecies() == "Parrot", true, "Bird species", __LINE__);
    
    return (status);
}