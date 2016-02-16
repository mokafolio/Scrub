#include <Stick/Test.hpp>
#include <Scrub/Shrub.hpp>

using namespace scrub;
using namespace stick;

const Suite spec[] =
{
    SUITE("Basic Tests")
    {
        Shrub s;
        s.set("a", "eins");
        s.set("a.b", "zwei");
        auto a = s.get<const String &>("a").ensure();
        auto b = s.get<const String &>("a.b").ensure();
        EXPECT(a == "eins");
        EXPECT(b == "zwei");
        EXPECT(s.child("a").ensure().value() == "eins");
        EXPECT(s.child("a").ensure().child("b").ensure().value() == "zwei");
        s.set("c", "drei");
        s.append(Shrub("e", "fünf"));
        s.append(Shrub("d", "vier"));
        EXPECT(s.count() == 4);

        String expectedOrder [4] = {"eins", "drei", "fünf", "vier"};
        Size i = 0;
        for (const auto & child : s)
        {
            EXPECT(child.value() == expectedOrder[i]);
            ++i;
        }
        s.sort();
        String expectedOrder2 [4] = {"eins", "drei", "vier", "fünf"};
        i = 0;
        for (const auto & child : s)
        {
            EXPECT(child.value() == expectedOrder2[i]);
            ++i;
        }
    },
    SUITE("Parse JSON Tests")
    {
        String testJSON =
            "{ \n"
            "   \"encoding\" : \"UTF-8\", \n"
            "   \"plug-ins\" : [ \n"
            "           \"python\", \n"
            "           \"c++\", \n"
            "           \"ruby\" \n"
            "           ], \n"
            "   \"indent\" : { \"length\" : 3, \"use_space\": true } \n"
            "} ";

        String failJSON = "{";

        Shrub tree = parseJSON(testJSON).ensure();
        EXPECT(tree.count() == 3);
        EXPECT(tree.child("encoding").ensure().value() == "UTF-8");
        EXPECT(tree.child("plug-ins").ensure().count() == 3);
        String expected[3] = {"python", "c++", "ruby"};
        Size i = 0;
        for(const auto & child : tree.child("plug-ins").ensure())
        {
            EXPECT(child.value() == expected[i]);
            ++i;
        }
        EXPECT(tree.child("indent").ensure().count() == 2);
        EXPECT(tree.get<Int32>("indent.length").ensure() == 3);
        EXPECT(tree.get<bool>("indent.use_space").ensure() == true);
        auto failTreeResult = parseJSON(failJSON);
        EXPECT(failTreeResult == false);
        EXPECT(failTreeResult.error() == ec::ParseFailed);
    }
};

int main(int _argc, const char * _args[])
{
    return runTests(spec, _argc, _args);
}
