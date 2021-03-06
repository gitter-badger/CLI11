#include "app_helper.hpp"

#include <cstdio>
#include <fstream>

TEST(Trim, Various) {
    std::string s1{"  sdlfkj sdflk sd s  "};
    std::string a1{"sdlfkj sdflk sd s"};
    CLI::detail::trim(s1);
    EXPECT_EQ(a1, s1);
    
    std::string s2{" a \t"};
    CLI::detail::trim(s2);
    EXPECT_EQ("a", s2);

    std::string s3{" a \n"};
    CLI::detail::trim(s3);
    EXPECT_EQ("a", s3);

    std::string s4{" a b "};
    EXPECT_EQ("a b", CLI::detail::trim(s4));

}


TEST(Validators, FileExists) {
    std::string myfile{"TestFileNotUsed.txt"};
    EXPECT_FALSE(CLI::ExistingFile(myfile));
    bool ok = static_cast<bool>(std::ofstream(myfile.c_str()).put('a')); // create file
    EXPECT_TRUE(ok);
    EXPECT_TRUE(CLI::ExistingFile(myfile));

    std::remove(myfile.c_str());
    EXPECT_FALSE(CLI::ExistingFile(myfile));
}

TEST(Validators, FileNotExists) {
    std::string myfile{"TestFileNotUsed.txt"};
    EXPECT_TRUE(CLI::NonexistentPath(myfile));
    bool ok = static_cast<bool>(std::ofstream(myfile.c_str()).put('a')); // create file
    EXPECT_TRUE(ok);
    EXPECT_FALSE(CLI::NonexistentPath(myfile));

    std::remove(myfile.c_str());
    EXPECT_TRUE(CLI::NonexistentPath(myfile));
}

// Yes, this is testing an app_helper :)
TEST(AppHelper, TempfileCreated) {
    std::string name = "TestFileNotUsed.txt";
    {
    TempFile myfile{name};

    EXPECT_FALSE(CLI::ExistingFile(myfile));
    
    bool ok = static_cast<bool>(std::ofstream(myfile.c_str()).put('a')); // create file
    EXPECT_TRUE(ok);
    EXPECT_TRUE(CLI::ExistingFile(name));
    EXPECT_THROW({TempFile otherfile(name);}, std::runtime_error);
    }
    EXPECT_FALSE(CLI::ExistingFile(name));
}

TEST(AppHelper, TempfileNotCreated) {
    std::string name = "TestFileNotUsed.txt";
    {
    TempFile myfile{name};

    EXPECT_FALSE(CLI::ExistingFile(myfile));
    }
    EXPECT_FALSE(CLI::ExistingFile(name));
}

TEST(AppHelper, Ofstream) {

    std::string name = "TestFileNotUsed.txt";
    {
    TempFile myfile(name);

    {
    std::ofstream out{myfile};
    out << "this is output" << std::endl;
    }

    EXPECT_TRUE(CLI::ExistingFile(myfile));
    }
    EXPECT_FALSE(CLI::ExistingFile(name));

}

TEST(Split, StringList) {

    std::vector<std::string> results {"a", "long", "--lone", "-q"};
    EXPECT_EQ(results, CLI::detail::split_names("a,long,--lone,-q"));

    EXPECT_EQ(std::vector<std::string>({"one"}), CLI::detail::split_names("one"));
}

TEST(RegEx, Shorts) {
    std::string name, value;

    EXPECT_TRUE(CLI::detail::split_short("-a", name, value));
    EXPECT_EQ("a", name);
    EXPECT_EQ("", value);

    EXPECT_TRUE(CLI::detail::split_short("-B", name, value));
    EXPECT_EQ("B", name);
    EXPECT_EQ("", value);

    EXPECT_TRUE(CLI::detail::split_short("-cc", name, value));
    EXPECT_EQ("c", name);
    EXPECT_EQ("c", value);

    EXPECT_TRUE(CLI::detail::split_short("-simple", name, value));
    EXPECT_EQ("s", name);
    EXPECT_EQ("imple", value);

    EXPECT_FALSE(CLI::detail::split_short("--a", name, value));
    EXPECT_FALSE(CLI::detail::split_short("--thing", name, value));
    EXPECT_FALSE(CLI::detail::split_short("--", name, value));
    EXPECT_FALSE(CLI::detail::split_short("something", name, value));
    EXPECT_FALSE(CLI::detail::split_short("s", name, value));
}

TEST(RegEx, Longs) {
    std::string name, value;

    EXPECT_TRUE(CLI::detail::split_long("--a", name, value));
    EXPECT_EQ("a", name);
    EXPECT_EQ("", value);

    EXPECT_TRUE(CLI::detail::split_long("--thing", name, value));
    EXPECT_EQ("thing", name);
    EXPECT_EQ("", value);

    EXPECT_TRUE(CLI::detail::split_long("--some=thing", name, value));
    EXPECT_EQ("some", name);
    EXPECT_EQ("thing", value);

    EXPECT_FALSE(CLI::detail::split_long("-a", name, value));
    EXPECT_FALSE(CLI::detail::split_long("-things", name, value));
    EXPECT_FALSE(CLI::detail::split_long("Q", name, value));
    EXPECT_FALSE(CLI::detail::split_long("--", name, value));

}

TEST(Regex, SplittingNew) {

    std::vector<std::string> shorts;
    std::vector<std::string> longs;
    std::string pname;

    EXPECT_NO_THROW(std::tie(shorts, longs, pname) = CLI::detail::get_names({"--long", "-s", "-q", "--also-long"}));
    EXPECT_EQ(std::vector<std::string>({"long", "also-long"}), longs);
    EXPECT_EQ(std::vector<std::string>({"s", "q"}), shorts);
    EXPECT_EQ("", pname);

    EXPECT_NO_THROW(std::tie(shorts, longs, pname) = CLI::detail::get_names({"--long", "", "-s", "-q", "", "--also-long"}));
    EXPECT_EQ(std::vector<std::string>({"long", "also-long"}), longs);
    EXPECT_EQ(std::vector<std::string>({"s", "q"}), shorts);

    EXPECT_THROW(std::tie(shorts, longs, pname) = CLI::detail::get_names({"-"}), CLI::BadNameString);
    EXPECT_THROW(std::tie(shorts, longs, pname) = CLI::detail::get_names({"--"}), CLI::BadNameString);
    EXPECT_THROW(std::tie(shorts, longs, pname) = CLI::detail::get_names({"-hi"}), CLI::BadNameString);
    EXPECT_THROW(std::tie(shorts, longs, pname) = CLI::detail::get_names({"one","two"}), CLI::BadNameString);

}
