#include <iostream>
#include <string>
#include <exception>
#include <vector>

class errInfo final {
public:
    int id{};
    int locationId{};
    int level{};
    std::string description;
    std::string reason;
};

class myException final : public std::exception {
public:
    explicit myException(std::string txt) :
            errMsg(std::move(txt)) {}

    [[nodiscard]] const char *what() const noexcept override {
        return errMsg.c_str();
    }

    void addErr(const errInfo &info) noexcept(false) {
        try {
            errList.push_back(info);
        }
        catch (...) {
            //push_back has thrown an error.
            std::cout << "Bad alloc in addErr function." << std::endl;
        }
    }

    std::vector<errInfo>::iterator begin() {
        return errList.begin();
    }

    std::vector<errInfo>::iterator end() {
        return errList.end();
    }

    ~myException() noexcept final {
        errList.clear();
    }

private:
    std::string errMsg;
    std::vector<errInfo> errList;

};

int main() {
    try {
        try {
            try {
                myException myex("myexception object thrown.");  //Create myexception object
                throw myex;   //Throw myexception object
            }
            catch (myException e) {    //Catch myexception object
                errInfo firstinfo;    //Create errorinfo struct
                firstinfo.reason = "level 1 error";
                firstinfo.id = 1;
                firstinfo.locationId = 0;
                firstinfo.level = 0;
                firstinfo.description = "First level exception thrown.";
                try {
                    e.addErr(firstinfo);  //Add errorinfo struct to myexception object
                }
                catch (std::bad_alloc&) {
                    std::cout << "Out of memory myexception.adderror bad_alloc." << std::endl;
                }
                throw e;   //Throw myexceptionobject up call stack.
            }
        }
        catch (myException& e) {
            errInfo secondinfo;
            secondinfo.reason = "level 2 error";
            secondinfo.id = 2;
            secondinfo.locationId = 1;
            secondinfo.level = 2;
            secondinfo.description = "Second level exception thrown.";
            try {
                e.addErr(secondinfo);
            }
            catch (std::bad_alloc&) {
                std::cout << "Out of memory myexception.adderror bad_alloc." << std::endl;
            }
            throw e;
        }
    }
    catch (myException& e) {
        //Print out errorinfo structs in myexception object
        for (std::vector<errInfo>::iterator iter = e.begin(); iter != e.end(); ++iter) {
            errInfo next = *iter;
            std::cout << next.reason << " " << next.id << " " << next.locationId << " " << next.level << " " << next.description << std::endl;
        }
    }

}
