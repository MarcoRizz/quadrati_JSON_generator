#ifndef FINDPATH_H
#define FINDPATH_H

using String = std::string;

extern void findPaths(int x, int y, int step, int max_size);
extern void findWordPaths(int x, int y, int step, String word, int wordIndex);

#endif