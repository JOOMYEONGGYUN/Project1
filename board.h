#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "page.h"

using std::endl;
using std::ofstream;
using std::vector;
using std::map;
using std::find;
using std::remove;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);
        void set_board(int x, int y, int w, int h, char cont);
        void board_insert(int x, int y, int w, int h, int id);
        void board_delete(int x, int y, int w, int h, int id);
        void delete_seq(int id, int save);
        void insert_seq(int id, int save);
        int find_min(int id);
        int find_max(int id);

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
        map <int, Page> pagemap;
        vector<int>* boardlst;
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];
    boardlst = new vector<int>[width * height];
    Page def = Page(' ');
    pagemap.insert({-1, def});

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
        }
    }

}

Board::~Board() {
    delete board;
    
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}


void Board::insert_page(int x, int y, int w, int h, int id, char c) {
    Page newpage = Page(x, y, w, h, id, c);
    pagemap.insert({id, newpage});
    board_insert(x, y, w, h, id);
    set_board(x, y, w, h, c);
    print_board();
}

void Board::delete_page(int id) {
    int save = id;
    delete_seq(id, save);
    insert_seq(id, save);
    board_delete(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].getw(), pagemap[id].geth(), id);
    pagemap.erase(id); 
}

void Board::modify_content(int id, char content) {
    int save = id;
    delete_seq(id, save);
    Page modpage = Page(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].getw(), pagemap[id].geth(), id, content);
    pagemap[id] = modpage;
    for (int h = pagemap[id].gety(); h < pagemap[id].gety() + pagemap[id].geth(); h++){
        for (int w = pagemap[id].getx(); w < pagemap[id].getx() + pagemap[id].getw(); w++){
            board[h * width + w] = content;
        }
    }
    print_board();
    insert_seq(id, save);
}

void Board::modify_position(int id, int x, int y) {
    int save = id;
    delete_seq(id, save);
    for (int h = y; h < y + pagemap[id].geth(); h++){
        for (int w = x; w < x + pagemap[id].getw(); w++){
            int k = h * width + w;
            for (auto itr = boardlst[k].begin(); itr != boardlst[k].end(); itr++){
                if (pagemap[boardlst[k][itr - boardlst[k].begin()]].getc() == board[k]){
                    boardlst[k].insert(itr + 1, id);
                    break;
                }
            }
            board[k] = pagemap[id].getc();
        }
    }
    print_board();
    insert_seq(id, save);
    for (int h = pagemap[id].gety(); h < pagemap[id].gety() + pagemap[id].geth(); h++){
        for (int w = pagemap[id].getx(); w < pagemap[id].getx() + pagemap[id].getw(); w++){
            int k = h * width + w;
            boardlst[k].erase(remove(boardlst[k].begin(), boardlst[k].end(), id));
        }
    }
    Page modpage = Page(x, y, pagemap[id].getw(), pagemap[id].geth(), id, pagemap[id].getc());
    pagemap[id] = modpage;
}

void Board::set_board(int x, int y, int w1, int h1, char cont){
    for (int h = y; h < y + h1; h++) {
        for (int w = x; w < x + w1; w++) {
            board[h*width + w] = cont;
        }
    }
}

void Board::board_insert(int x, int y, int w1, int h1, int id){
    for (int h = y; h < y + h1; h++){
        for (int w = x; w < x + w1; w++){
            boardlst[h * width + w].push_back(id);
        }
    }
}

void Board::board_delete(int x, int y, int w1, int h1, int id){
    for (int h = y; h < y + h1; h++){
        for (int w = x; w < x + w1; w++){
            int k = h * width + w;
            boardlst[k].erase(find(boardlst[k].begin(), boardlst[k].end(), id));
        }
    }
}

void Board::delete_seq(int id, int save){
    if (find_min(id) == 32768){
        for (int h = pagemap[id].gety(); h < pagemap[id].gety() + pagemap[id].geth(); h++) {
            for (int w = pagemap[id].getx(); w < pagemap[id].getx() + pagemap[id].getw(); w++) {
                int j = h * width + w;
                board[j] = pagemap[boardlst[j][find(boardlst[j].begin(), boardlst[j].end(), id) - boardlst[j].begin() - 1]].getc();
            }
        }
        print_board();
    } 
    else{
        delete_seq(find_min(id), save);
        delete_seq(id, save);
    }
}

void Board::insert_seq(int id, int save){
    if (find_max(id) != -1){ 
        if (id != save){ 
            set_board(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].getw(), pagemap[id].geth(), pagemap[id].getc());
            print_board();
            }
        insert_seq(find_max(id), save);
        insert_seq(id, save);
    }
    else{
        if (id != save){
            for (int h = pagemap[id].gety(); h < pagemap[id].gety() + pagemap[id].geth(); h++){
                for (int w = pagemap[id].getx(); w < pagemap[id].getx() + pagemap[id].getw(); w++){
                    if (board[h * width + w] == pagemap[id].getc()){
                        return;
                    }
                }
            }
            set_board(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].getw(), pagemap[id].geth(), pagemap[id].getc());
            print_board();
        }
    }
}