#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <string>
#include <sstream>

using namespace std;

class BrowserHistory {
private:
    stack<string> backStack, forwardStack;
    vector<pair<string, string> > history;
    map<string, int> visitCount;
    map<string, set<string> > categorizedBookmarks;
    string currentPage;
    bool incognito;
    string bookmarkPassword;

    string getCurrentTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        dt[strlen(dt) - 1] = '\0'; // remove newline
        return string(dt);
    }

public:
    BrowserHistory() {
        incognito = false;
        bookmarkPassword = "admin";
    }

    void visitSite(const string& url) {
        if (!currentPage.empty()) backStack.push(currentPage);
        while (!forwardStack.empty()) forwardStack.pop();
        currentPage = url;
        if (!incognito) {
            history.push_back(make_pair(url, getCurrentTime()));
            visitCount[url]++;
        }
        cout << "Visited: " << url << endl;
    }

    void goBack() {
        if (backStack.empty()) {
            cout << "No pages to go back to.\n";
            return;
        }
        forwardStack.push(currentPage);
        currentPage = backStack.top();
        backStack.pop();
        cout << "Current Page: " << currentPage << endl;
    }

    void goForward() {
        if (forwardStack.empty()) {
            cout << "No pages to go forward to.\n";
            return;
        }
        backStack.push(currentPage);
        currentPage = forwardStack.top();
        forwardStack.pop();
        cout << "Current Page: " << currentPage << endl;
    }

    void showCurrentPage() {
        if (currentPage.empty())
            cout << "No page loaded.\n";
        else
            cout << "Current Page: " << currentPage << endl;
    }

    void showHistory() {
        if (history.empty()) {
            cout << "History is empty.\n";
            return;
        }
        cout << "Full Browsing History:\n";
        for (size_t i = 0; i < history.size(); i++) {
            cout << history[i].first << " at " << history[i].second << endl;
        }
    }

    void searchHistory(const string& keyword) {
        bool found = false;
        for (size_t i = 0; i < history.size(); i++) {
            if (history[i].first.find(keyword) != string::npos) {
                cout << "Found: " << history[i].first << " at " << history[i].second << endl;
                found = true;
            }
        }
        if (!found) cout << "No matching entries found.\n";
    }

    void bookmarkCurrentSite() {
        if (currentPage.empty()) {
            cout << "No page loaded to bookmark.\n";
            return;
        }
        string category;
        cout << "Enter category for this bookmark: ";
        getline(cin, category);
        categorizedBookmarks[category].insert(currentPage);
        cout << "Bookmarked '" << currentPage << "' under category '" << category << "'.\n";
    }

    void showBookmarks() {
        string input;
        cout << "Enter bookmark password: ";
        getline(cin, input);
        if (input != bookmarkPassword) {
            cout << "Incorrect password. Access denied.\n";
            return;
        }

        if (categorizedBookmarks.empty()) {
            cout << "No bookmarks saved.\n";
            return;
        }

        map<string, set<string> >::iterator it;
        for (it = categorizedBookmarks.begin(); it != categorizedBookmarks.end(); ++it) {
            cout << "Category: " << it->first << "\n";
            set<string>::iterator sit;
            for (sit = it->second.begin(); sit != it->second.end(); ++sit) {
                cout << " - " << *sit << "\n";
            }
        }
    }

    void deleteFromHistory(const string& url) {
        vector<pair<string, string> >::iterator it = history.begin();
        while (it != history.end()) {
            if (it->first == url)
                it = history.erase(it);
            else
                ++it;
        }
        visitCount.erase(url);
        cout << "Deleted: " << url << " from history.\n";
    }

    void exportHistory(const string& filename) {
        ofstream out(filename.c_str());
        for (size_t i = 0; i < history.size(); i++) {
            out << history[i].first << "," << history[i].second << endl;
        }
        out.close();
        cout << "History exported to " << filename << endl;
    }

    void importHistory(const string& filename) {
        ifstream in(filename.c_str());
        string line, url, timestamp;
        while (getline(in, line)) {
            size_t comma = line.find(',');
            if (comma != string::npos) {
                url = line.substr(0, comma);
                timestamp = line.substr(comma + 1);
                history.push_back(make_pair(url, timestamp));
                visitCount[url]++;
            }
        }
        in.close();
        cout << "History imported from " << filename << endl;
    }

    void showSessionSummary() {
        cout << "Total sites visited: " << history.size() << endl;
        if (!history.empty())
            cout << "Most recent visit: " << history.back().first << endl;

        vector<pair<int, string> > freq;
        map<string, int>::iterator it;
        for (it = visitCount.begin(); it != visitCount.end(); ++it) {
            freq.push_back(make_pair(it->second, it->first));
        }

        sort(freq.rbegin(), freq.rend());
        cout << "Top 3 most visited sites:\n";
        for (size_t i = 0; i < freq.size() && i < 3; ++i) {
            cout << freq[i].second << " - " << freq[i].first << " times\n";
        }
    }

    void toggleIncognitoMode() {
        incognito = !incognito;
        cout << (incognito ? "Incognito Mode Enabled" : "Incognito Mode Disabled") << endl;
    }
};

int main() {
    BrowserHistory browser;
    int choice;
    string url, keyword, filename, input;

    while (true) {
        cout << "\n==============================\n"
             << "Browser History Manager\n"
             << "==============================\n"
             << "1. Visit Site\n"
             << "2. Go Back\n"
             << "3. Go Forward\n"
             << "4. Show Current Page\n"
             << "5. Show History\n"
             << "6. Search History\n"
             << "7. Bookmark Current Site\n"
             << "8. Show Bookmarks (Password Required)\n"
             << "9. Delete from History\n"
             << "10. Export History to File\n"
             << "11. Import History from File\n"
             << "12. Session Summary\n"
             << "13. Toggle Incognito Mode\n"
             << "0. Exit\n"
             << "Enter your choice: ";

        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> choice) || (choice < 0 || choice > 13)) {
            cout << "Invalid input. Are you sure you want to exit? (yes/no): ";
            string confirm;
            getline(cin, confirm);
            if (confirm == "yes" || confirm == "Yes" || confirm == "YES") {
                cout << "Exiting browser history manager. Goodbye!\n";
                break;
            } else {
                cout << "Continuing...\n";
                continue;
            }
        }

        switch (choice) {
            case 1:
                cout << "Enter URL to visit: ";
                getline(cin, url);
                browser.visitSite(url);
                break;
            case 2:
                browser.goBack();
                break;
            case 3:
                browser.goForward();
                break;
            case 4:
                browser.showCurrentPage();
                break;
            case 5:
                browser.showHistory();
                break;
            case 6:
                cout << "Enter keyword to search in history: ";
                getline(cin, keyword);
                browser.searchHistory(keyword);
                break;
            case 7:
                browser.bookmarkCurrentSite();
                break;
            case 8:
                browser.showBookmarks();
                break;
            case 9:
                cout << "Enter URL to delete from history: ";
                getline(cin, url);
                browser.deleteFromHistory(url);
                break;
            case 10:
                cout << "Enter filename to export history to: ";
                getline(cin, filename);
                browser.exportHistory(filename);
                break;
            case 11:
                cout << "Enter filename to import history from: ";
                getline(cin, filename);
                browser.importHistory(filename);
                break;
            case 12:
                browser.showSessionSummary();
                break;
            case 13:
                browser.toggleIncognitoMode();
                break;
            case 0:
                cout << "Exiting browser history manager. Goodbye!\n";
                return 0;
                
            default: {
    string confirm;
    cout << "Invalid input. Are you sure you want to exit? (yes/no): ";
    getline(cin, confirm);
    if (confirm == "yes" || confirm == "Yes" || confirm == "YES") {
        choice = 0; 
        cout << "Exiting browser history manager. Goodbye!\n";
    } else {
        cout << "Continuing...\n";
        choice = 999;
    }
    break;
}
    
        }
    }

    return 0;
}
