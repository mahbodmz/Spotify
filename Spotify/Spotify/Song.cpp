#include "Song.h"
#include <iostream>

Song::Song() {
    title = "";
    artist = "";
    releaseDate = "";
    genre = "";
    duration = 0;
}
Song::Song(string t,string a,string r,string g,int d) {
    title = t;
    artist = a;
    releaseDate = r;
    genre = g;
    duration = d;
}
void Song::setTitle(const string& t) { title = t; }
void Song::setArtist(const string& a) { artist = a; }
void Song::setReleaseDate(const string& r) { releaseDate = r; }
void Song::setGenre(const string& g) { genre = g; }
void Song::setDuration(int d) { duration = d; }

string Song::getTitle() const { return title; }
string Song::getArtist() const { return artist; }
string Song::getReleaseDate() const { return releaseDate; }
string Song::getGenre() const { return genre; }
int Song::getDuration() const { return duration; }

void Song::display() const {
    cout << "Title: " << title << "\n";
    cout << "Artist: " << artist << "\n";
    cout << "Release Date: " << releaseDate << "\n";
    cout << "Genre: " << genre << "\n";
    cout << "Duration: " << duration << " seconds\n";
}