#ifndef SONG_H
#define SONG_H
#include <iostream>
using namespace std;
class Song {
private:
	string title;
	string artist;
	string releaseDate;
	string genre;
	int duration;

public:
	Song();
	Song(string t, string a, string r, string g, int d);
	void setTitle(const string& t);
	void setArtist(const string& a);
	void setReleaseDate(const string& r);
	void setGenre(const string& g);
	void setDuration( int d);

	string getTitle() const;
	string getArtist() const;
	string getReleaseDate() const;
	string getGenre() const;
	int getDuration() const;

	void display() const;
};
#endif
