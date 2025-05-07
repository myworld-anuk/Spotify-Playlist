#include <fstream>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

void insert_output(ofstream &output_file, const vector<string> &songs)
{
    for (const string &song : songs)
    {
        output_file << song << endl;
        // insert each song into playlist
    }
}

void next_song(vector<string> &songs, int &current_song_index)
{
    if (!songs.empty() && current_song_index >= 0 && current_song_index < songs.size())
    {
        string song_line = songs[current_song_index];
        int index_of_curr = song_line.find("current");
        songs[current_song_index] = song_line.substr(0, index_of_curr - 1); // Remove " current" from current song

        current_song_index = (current_song_index + 1) % songs.size(); // Move to the next song
        songs[current_song_index] += " current";                      // Add "current" to the next song
    }
}

void previous_song(vector<string> &songs, int &current_song_index)
{
    if (!songs.empty() && current_song_index >= 0 && current_song_index < songs.size())
    {
        string song_line = songs[current_song_index];
        int index_of_curr = song_line.find("current");
        songs[current_song_index] = song_line.substr(0, index_of_curr - 1); // Remove " current"

        current_song_index = (current_song_index - 1 + songs.size()) % songs.size(); // Move to the previous song
        songs[current_song_index] += " current";                                     // Add "current" to the previous song
    }
}

void add_song(string action, vector<string> &songs)
{
    string new_song = action.substr(4); // Remove "add "
    songs.push_back(new_song);          // Add the new song to the playlist
}

void remove_song(string action, vector<string> &songs, int &current_song_index)
{
    string remove_song = action.substr(7); // Remove "remove "
    for (int i = 0; i < songs.size(); i++)
    {
        if (songs[i] == remove_song + " current") // if song to be removed is current song case
        {
            current_song_index = (current_song_index - 1 + songs.size()) % songs.size();
            songs[current_song_index] += " current";
        }
        if (songs[i] == remove_song) // case for non_current songs
        {
            songs.erase(songs.begin() + i);
            if (i <= current_song_index && current_song_index > 0)
            {
                current_song_index--;
            }
            break;
        }
    }
}

void move_song(string action, vector<string> &songs, int &current_song_index)
{
    int ind = action.rfind(" ");              // finds the last space in action statement
    int pos = stoi(action.substr(ind + 1));   // converts last digits to int
    string move_song = action.substr(0, ind); // cuts action statement to only include song name and artist
    move_song = move_song.substr(5, (move_song.size() - 5));

    for (int i = 0; i < songs.size(); i++)
    {
        if (songs[i] == move_song + " current") // case for if song to be moved is current
        {
            songs[i] = move_song;                                      // Remove " current" tag from old position
            songs.insert(songs.begin() + pos, move_song + " current"); // Add "current" tag at the new position
            int erase_index;

            //calculates old position
            if (i < pos) 
            {
                erase_index = i;
            } 
            else 
            {
                erase_index = i + 1;
            }
            songs.erase(songs.begin() + erase_index); //removes           
            current_song_index = pos;
            return;
        }
        else if (songs[i] == move_song) // case for non-current songs
        {
            songs.erase(songs.begin() + i);
            songs.insert(songs.begin() + pos, move_song);
            if (i < current_song_index && pos >= current_song_index)
            {
                current_song_index--;
            }
            else if (i > current_song_index && pos <= current_song_index)
            {
                current_song_index++;
            }
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    // getting file names and opening
    string playlist_name = argv[1];
    ifstream playlist_file(playlist_name);


    string actions_name = argv[2];
    ifstream actions_file(actions_name);

    string output_name = argv[3];
    ofstream output_file(output_name);

    if (!playlist_file.is_open() || !actions_file.is_open() || !output_file.is_open())
    {
        cerr << "Failed to open one or more files" << endl;
        return 1;
    }

    vector<string> songs;
    string playlist_line;
    int current_song_index = -1;

    while (getline(playlist_file, playlist_line)) // adding playlist songs to song vector
    {
        songs.push_back(playlist_line);
        if (playlist_line.find("current") != string::npos)
        {
            current_song_index = songs.size() - 1;
        }
    }

    string action;
    while (getline(actions_file, action)) // iterate through every action and comeplete
    {
        if (action == "next")
        {
            next_song(songs, current_song_index);
        }
        else if (action == "previous")
        {
            previous_song(songs, current_song_index);
        }
        else if (action.find("add") == 0)
        {
            add_song(action, songs);
        }
        else if (action.find("remove") == 0)
        {
            remove_song(action, songs, current_song_index);
        }
        else
        {
            move_song(action, songs, current_song_index);
        }
    }

    insert_output(output_file, songs);

    // closing all files
    playlist_file.close();
    actions_file.close();
    output_file.close();

    return 0;
}