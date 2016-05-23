#include <Stick/Test.hpp>
#include <Scrub/Shrub.hpp>

using namespace scrub;
using namespace stick;

const Suite spec[] =
{
    SUITE("Basic Tests")
    {
        Shrub s;
        s.set("a", String("eins"));
        s.set("a.b", "zwei");
        auto a = s.maybe<const String &>("a").ensure();
        auto b = s.maybe<const String &>("a.b").ensure();
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
        EXPECT(tree.child("encoding").ensure().valueHint() == ValueHint::JSONString);
        EXPECT(tree.child("plug-ins").ensure().count() == 3);
        EXPECT(tree.child("plug-ins").ensure().valueHint() == ValueHint::JSONArray);
        String expected[3] = {"python", "c++", "ruby"};
        Size i = 0;
        for (const auto & child : tree.child("plug-ins").ensure())
        {
            EXPECT(child.value() == expected[i]);
            ++i;
        }
        EXPECT(tree.child("indent").ensure().count() == 2);
        EXPECT(tree.maybe<Int32>("indent.length").ensure() == 3);
        EXPECT(tree.child("indent.length").ensure().valueHint() == ValueHint::JSONInt);
        EXPECT(tree.maybe<bool>("indent.use_space").ensure() == true);
        auto failTreeResult = parseJSON(failJSON);
        EXPECT(failTreeResult == false);
        EXPECT(failTreeResult.error() == ec::ParseFailed);
    },
    SUITE("Parse XML Tests")
    {
        String testXML =
            "<debug version = '1.3'>\n"
            "<filename>debug.log</filename>\n"
            "<modules>\n"
            "<module>Finance</module>\n"
            "<module>Admin</module>\n"
            "<module>HR</module>\n"
            "</modules>\n"
            "<level>2</level>\n"
            "</debug>\n";

        String invalidXML =
            "<start>";

        Shrub tree = parseXML(testXML).ensure();
        EXPECT(tree.count() == 4);
        EXPECT(tree.maybe<const String &>("filename").ensure() == "debug.log");
        EXPECT(tree.child("filename").ensure().count() == 0);
        EXPECT(tree.child("modules").ensure().count() == 3);
        String expected[3] = {"Finance", "Admin", "HR"};
        Size i = 0;
        for(const auto & child : tree.child("modules").ensure())
        {
            EXPECT(child.value() == expected[i]);
            ++i;
        }
        EXPECT(tree.get<Int32>("level") == 2);

        auto broken = parseXML(invalidXML);
        EXPECT(broken == false);
        EXPECT(broken.error() == ec::ParseFailed);
    }
};

int main(int _argc, const char * _args[])
{
    return runTests(spec, _argc, _args);
}
