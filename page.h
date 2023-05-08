class Page {
    public:
        Page(int x, int y, int width, int height, int id, char content): x(x), y(y), width(width), height(height), id(id), content(content) {}
        Page(char content): content(content){}
        Page(){};
        int getx(){return x;}
        int gety(){return y;}
        int getw(){return width;}
        int geth(){return height;}
        int get_id(){return id;}
        char getc(){return content;}
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content; 
};
