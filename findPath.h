#ifndef FINDPATH_H
#define FINDPATH_H

extern void findPaths(int x, int y, int step, int max_size);
extern void findWordPaths(int x, int y, int step, std::string word, int wordIndex, std::pair<int, int>& startingtile);
bool is_still_in_grid(int x, int y, int step, const std::string& word);

#endif