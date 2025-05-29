#include "core/FileBrowser.h"
#include <iostream>
#include <algorithm> 

FileBrowser::FileBrowser()
    : selectedIndex(0), scrollOffset(0), visibleItemsCount(0) {
    currentPath = std::filesystem::current_path();
    listDirectory(currentPath);
}

void FileBrowser::listDirectory(const std::filesystem::path& path) {
    currentItems.clear();
    selectedIndex = 0;
    scrollOffset = 0;

    try {
        std::vector<FileItem> tempItems; // Use a temporary vector for sorting

        // Add ".." for parent directory if not at root
        if (path.has_parent_path() && path != path.root_path()) {
            tempItems.push_back({"..", true}); // ".." is always a directory
        }

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            tempItems.push_back({
                entry.path().filename().string(),
                std::filesystem::is_directory(entry.path())
            });
        }
        

        std::sort(tempItems.begin(), tempItems.end(), [](const FileItem& a, const FileItem& b) {
            // Special handling for ".." to always be at the top
            if (a.name == ".." && b.name != "..") return true;
            if (a.name != ".." && b.name == "..") return false;
            
            // Directories before files
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory > b.isDirectory;
            }
            // Alphabetical for same type
            return a.name < b.name;
        });

        currentItems = tempItems; // Assign the sorted temporary vector

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void FileBrowser::selectNextItem() {
    if (currentItems.empty()) return;

    selectedIndex++;
    if (selectedIndex >= currentItems.size()) {
        selectedIndex = currentItems.size() - 1;
    }

    if (selectedIndex >= scrollOffset + visibleItemsCount) {
        scrollOffset = selectedIndex - visibleItemsCount + 1;
    }
}

void FileBrowser::selectPreviousItem() {
    if (currentItems.empty()) return;

    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = 0;
    }

    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    }
}

void FileBrowser::tryOpenSelectedItem() {
    if (currentItems.empty()) return;

    const FileItem& selectedItem = currentItems[selectedIndex];
    std::string selectedName = selectedItem.name;

    std::filesystem::path newPath = currentPath / selectedName;

    if (selectedName == "..") {
        goUpDirectory();
    } else if (selectedItem.isDirectory) { 
        currentPath = newPath;
        listDirectory(currentPath);
    } else { 
        std::cout << "Selected file: " << newPath.string() << std::endl;
    }
}

void FileBrowser::goUpDirectory() {
    if (currentPath.has_parent_path() && currentPath != currentPath.root_path()) {
        currentPath = currentPath.parent_path();
        listDirectory(currentPath);
    } else {
        std::cout << "Already at the root directory." << std::endl;
    }
}

void FileBrowser::setVisibleItemsCount(int count) {
    visibleItemsCount = count;
}
