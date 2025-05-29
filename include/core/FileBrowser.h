#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <string>
#include <vector>
#include <filesystem>

struct FileItem {
    std::string name;
    bool isDirectory;
};

class FileBrowser {
public:
    FileBrowser();

    std::string getCurrentPath() const { return currentPath.string(); }
    const std::vector<FileItem>& getCurrentItems() const { return currentItems; }
    int getSelectedIndex() const { return selectedIndex; }
    int getScrollOffset() const { return scrollOffset; }
    int getVisibleItemsCount() const { return visibleItemsCount; }

    void selectNextItem();
    void selectPreviousItem();
    void tryOpenSelectedItem();
    void goUpDirectory();

    void setVisibleItemsCount(int count);

private:
    std::filesystem::path currentPath;
    std::vector<FileItem> currentItems;
    int selectedIndex;
    int scrollOffset;

    int visibleItemsCount;

    void listDirectory(const std::filesystem::path& path);
};

#endif // FILEBROWSER_H
