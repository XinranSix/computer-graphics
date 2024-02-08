#include <fstream>
#include "nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    {
        std::ifstream f { "example.json" };
        json data = json::parse(f);
    }

    {
        // Using (raw) string literals and json::parse
        json ex1 = json::parse(R"(
                {
                    "pi": 3.141,
                    "happy": true
                }
)");

        // Using user-defined (raw) string literals
        using namespace nlohmann::literals;
        json ex2 = R"(
            {
                "pi": 3.141,
                "happy": true
            }
)"_json;

        // Using initializer lists
        json ex3 = {
            { "happy", true },
            { "pi", 3.141 },
        };
    }

    { // create an empty structure (null)
        json j;
        // add a number that is stored as double (note the implicit conversion of j to an object)
        j["pi"] = 3.141;
        // add a Boolean that is stored as bool
        j["happy"] = true;
        // add a string that is stored as std::string
        j["name"] = "Niels";
        // add another null object by passing nullptr
        j["nothing"] = nullptr;
        // add an object inside the object
        j["answer"]["everything"] = 42;
        // add an array that is stored as std::vector (using an initializer list)
        j["list"] = { 1, 0, 2 };
        // add another object (using an initializer list of pairs)
        j["object"] = { { "currency", "USD" }, { "value", 42.99 } };
        // instead, you could also write (which looks very similar to the JSON above)
        json j2 = { { "pi", 3.141 },
                    { "happy", true },
                    { "name", "Niels" },
                    { "nothing", nullptr },
                    { "answer", { { "everything", 42 } } },
                    { "list", { 1, 0, 2 } },
                    { "object", { { "currency", "USD" }, { "value", 42.99 } } } };
    }

    { // a way to express the empty array []
        json empty_array_explicit = json::array();

        // ways to express the empty object {}
        json empty_object_implicit = json({});
        json empty_object_explicit = json::object();

        // a way to express an _array_ of key/value pairs [["currency", "USD"], ["value", 42.99]]
        json array_not_object = json::array({ { "currency", "USD" }, { "value", 42.99 } });
    }

    { // create object from string literal
        json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;

        // or even nicer with a raw string literal
        auto j2 = R"(
  {
    "happy": true,
    "pi": 3.141
  }
)"_json;
    }

    {}
    {}
    {}
    {}


    return 0;
}
