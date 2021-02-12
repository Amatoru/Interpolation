/* Thêm các hàm output ra file
 * Hiển thị nội dung file ra màn hình */

/* Khai báo thư viện */
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>

/* Khai báo hằng số */
#define N_CHOICES 5
#define MENU_HEIGHT 7
#define MENU_WIDTH 40
#define INPUT_HEIGHT 7
#define INPUT_WIDTH (COLS - MENU_WIDTH - 10)
#define OUTPUT_HEIGHT (LINES - MENU_HEIGHT -3)
#define OUTPUT_WIDTH (COLS - 5)

const char *choices[] = {
                            "1. Nhap bang so lieu",
                            "2. Noi suy Lagrange",
                            "3. Kiem tra lai bang luoc do Horner",
                            "4, Binh phuong sai so",
                            "5. Hien thi ket qua tu file roi thoat",
                        };

/* Khai báo hàm */
// Các hàm in và nhập
void print_menu(WINDOW *menu_win, int highlight);
bool print_output(WINDOW *input_win, WINDOW *output_win, int choice, double X[], double Y[], double L[30][30],
                  int nInput, int nDigit, int *fullDeg, bool *haveF, double **F, FILE **fp);
int nhapBangSoLieu(WINDOW *input_win, double X[], double Y[]);
void print_input(WINDOW *input_win, double X[], double Y[], int nInput, int nDigit, FILE **fp);
void print_precision(WINDOW *win, double num, int nDigit, FILE **fp);
void printPoly(WINDOW *win, double poly[], int n, int nDigit, FILE **fp);
// Lagrange
double *multiPoly(double A[], double B[], int m, int n);
double *lagrange(WINDOW *win, double X[], double Y[], double L[30][30], int nInput, int nDigit, FILE **fp);
// Horner
double horner(WINDOW *win, double F[], double x, int fullDeg, int nDigit, FILE **fp);
// Bình phương sai số
double *gauss(double *matrix, int n);
double *lsq(WINDOW *win, double X[], double Y[], int nInput, int nDigit, int deg, FILE **fp);


int main(void)
{
    WINDOW *menu_win;
    WINDOW *input_win;
    WINDOW *output_win;
    FILE *fp;
    int highlight = 1;
    int choice;
    int move;
    bool quit = FALSE;
    // input variables
    double X[30];
    double Y[30];
    double L[30][30];
    int nInput = 0;
    int nDigit;
    double *F;
    bool haveF;
    int fullDeg;
    char c;
    int y, x;


    fp = fopen("log.txt", "w");
    initscr();
    clear();
    noecho();
    cbreak();

    input_win = newwin(INPUT_HEIGHT, INPUT_WIDTH, 2, MENU_WIDTH + 7);
    output_win = newwin(OUTPUT_HEIGHT, OUTPUT_WIDTH, 10, 2);
    box(input_win, 0 , 0);
    box(output_win, 0, 0);
    refresh();
    wrefresh(input_win);
    wrefresh(output_win);

    menu_win = newwin(MENU_HEIGHT, MENU_WIDTH, 2, 2);
    keypad(menu_win, TRUE);
    mvprintw(1, MENU_WIDTH / 2 - 3, "MENU");
    mvprintw(1, MENU_WIDTH + INPUT_WIDTH / 2 , "INPUT");
    mvprintw(9, COLS/2 - 3, "OUTPUT");

    refresh();
    print_menu(menu_win, highlight);
    while (1)
    {
        choice = 0;
        while (1)
        {
            move = wgetch(menu_win);
            switch(move)
            {
                case KEY_UP:
                    if (highlight == 1)
                        highlight = N_CHOICES;
                    else
                        --highlight;
                    break;
                case KEY_DOWN:
                    if (highlight == N_CHOICES)
                        highlight = 1;
                    else
                        ++highlight;
                    break;
                case 10:
                    choice = highlight;
                    break;
                default:
                    break;
            }
            print_menu(menu_win, highlight);
            if (choice != 0)
                break;
        }
        /* Hiển thị kết quả ra màn hình */
        if (choice == 1)
        {
            haveF = FALSE;
            // Nhập nDigit
            echo(); nocbreak();
            wclear(input_win);
            box(input_win, 0, 0);
            mvwprintw(input_win, 1, 1, "So chu so dang sau dau phay (> 0, < 16): ");
            wscanw(input_win, "%d", &nDigit);
            wrefresh(input_win);
            while (nDigit < 1 || nDigit > 15)
            {
                wclear(input_win);
                box(input_win, 0, 0);
                wrefresh(input_win);
                mvwprintw(input_win, 1, 1, "So chu so dang sau dau phay (> 0, < 16): ");
                wscanw(input_win, "%d", &nDigit);
            }
            // Nhập bảng số liệu
            nInput = nhapBangSoLieu(input_win, X, Y);
            print_input(input_win, X, Y, nInput, nDigit, &fp);
        }
        else
        {
            quit = print_output(input_win, output_win, choice, X, Y, L, nInput, nDigit, &fullDeg, &haveF, &F, &fp);
            wrefresh(output_win);
            if (quit) // Người dùng chọn hiển thị file rôi thoát
            {
                fclose(fp);
                // Đọc lại file vừa ghi rồi in lên màn hình
                fp = fopen("log.txt", "r");
                if (fp != NULL)
                {
                    mvwprintw(output_win, 1, 1, "%s", choices[4]);
                    getyx(output_win, y, x);
                    wmove(output_win, y+1, 1);
                    c = fgetc(fp);
                    while (c != EOF)
                    {
                        if (c == '\n')
                        {
                            c = fgetc(fp);
                            getyx(output_win, y, x);
                            wmove(output_win, y+1, 1);
                            wrefresh(output_win);

                            if (y == OUTPUT_HEIGHT -3)  // Hết dòng
                            {
                                mvwprintw(output_win, y+1, 1, "<Enter> to see more");
                                wgetch(output_win);
                                wclear(output_win);
                                box(output_win, 0, 0);
                                wmove(output_win, 1, 1);
                                wrefresh(output_win);
                            }

                            continue;
                        }
                        wprintw(output_win, "%c", c);
                        c = fgetc(fp);
                    }
                    wgetch(output_win);
                }
                fclose(fp);
                break;
            }
        }

    }
    endwin();

    return 0;
}


/* Định nghĩa hàm */

void print_menu(WINDOW *menu_win, int highlight)
{
    int x, y, i;

    x = 2;
    y = 1;
    box(menu_win, 0, 0);
    for (i = 0; i < N_CHOICES; ++i)
    {
        if (highlight == i+1)
        {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

bool print_output(WINDOW *input_win, WINDOW *output_win, int choice, double X[], double Y[], double L[30][30],
                  int nInput, int nDigit, int *fullDeg, bool *haveF, double **F, FILE **fp)
{
    int y, x;
    int deg;
    double x0;
    double F_x;
    int i;

    wclear(output_win);
    box(output_win, 0, 0);
if (nInput != 0)
    {
        switch(choice)
        {
            case 2:     /* Nội suy Lagrange */
                fprintf(*fp,"\n\n>>> Tim ham noi suy Lagrange:\n");
                *haveF = TRUE;
                *fullDeg = nInput;
                mvwprintw(output_win, 1, 1, "%s", choices[1]);
                getyx(output_win, y, x);
                wmove(output_win, y+1, 1);
                wprintw(output_win, "Da thuc Lagrange co ban:");
                fprintf(*fp, "Da thuc Lagrange co ban:\n");
                wmove(output_win, y+2, 1);

                wprintw(output_win, "Li(x) = ");
                wattron(output_win, A_UNDERLINE);
                wprintw(output_win, "   (x - x1)(x - x2)...(x - xi-1)(x - xi+1)...(x - xn)  ");
                wattroff(output_win, A_UNDERLINE);
                wmove(output_win, y+3, 11);
                wprintw(output_win, "(xi - x1)(xi - x2)...(xi - xi-1)(xi - xi+1)...(xi - xn)");
                *F = lagrange(output_win, X, Y, L, nInput, nDigit, &(*fp));

                getyx(output_win, y, x);
                wmove(output_win, y+2, 1);
                wprintw(output_win, "Da thuc noi suy Lagrange: F(x) = \u03A3(yi*Li)");
                wmove(output_win, y+3, 34);
                wprintw(output_win, "i");
                wmove(output_win, y+4, 1);
                wprintw(output_win, "===> F(x) = ");
                fprintf(*fp, "===> F(x) = ");
                printPoly(output_win, *F, nInput, nDigit, &(*fp));


                getyx(input_win, y, x);
                wmove(input_win, y, 1);
                wclrtoeol(input_win);
                box(input_win, 0, 0);
                wrefresh(input_win);
                mvwprintw(input_win, y, 1, "Da thuc noi suy tim duoc: F(x) = ");
                printPoly(input_win, *F, nInput, nDigit, NULL);
                wrefresh(input_win);


                break;
            case 3:     /* Lược đồ Horner */
                if (*haveF)
                {
                    // Lấy x0 từ bàn phím
                    mvwprintw(output_win, 1, 1, "%s", choices[2]);
                    mvwprintw(output_win, 2, 1, "x0 = ");
                    nocbreak(); echo();
                    while (wscanw(output_win, "%lf", &x0) != 1)
                    {
                        wmove(output_win, 2, 1);
                        wclrtoeol(output_win);
                        box(output_win, 0, 0);
                        mvwprintw(output_win, 2, 1, "x0 = ");
                    }
                    noecho(); cbreak();
                    // Dùng lược đồ Horner tính giá trị các đa thức cơ bản và đa thức nội suy
                    fprintf(*fp, "\n\n>>> Tinh gia tri cac da thuc Lagrange bang luoc do horner tai diem x0 = %.3lf", x0);
                    for (i = 0; i < nInput; i++)
                    {
                        fprintf(*fp, "\n\n- L%d", i+1);
                        F_x = horner(output_win, L[i], x0, *fullDeg, nDigit, &(*fp));
                        getyx(output_win, y, x);
                        mvwprintw(output_win, y+1, 1, "===> L%d(%.3lf) = ", i+1, x0);
                        fprintf(*fp, "\n===> L%d(%.3lf) = ", i+1, x0);
                        print_precision(output_win, F_x, nDigit, &(*fp));
                        mvwprintw(output_win, OUTPUT_HEIGHT-2, 1, "Nhan enter de chuyen sang ham tiep theo");
                        wgetch(output_win);
                        wclear(output_win);
                        box(output_win, 0, 0);
                        wrefresh(output_win);
                        mvwprintw(output_win, 1, 1, "%s", choices[2]);
                        mvwprintw(output_win, 2, 1, "x0 = %.3lf", x0);
                        wmove(output_win, 3, 1);
                    }
                    fprintf(*fp, "\n\n- F(x)");
                    F_x = horner(output_win, *F, x0, *fullDeg, nDigit, &(*fp));
                    getyx(output_win, y, x);
                    mvwprintw(output_win, y+1, 1, "===> F(%.3lf) = ", x0);
                    fprintf(*fp, "\n===> F(%.3lf) = ", x0);
                    print_precision(output_win, F_x, nDigit, &(*fp));
                }
                else
                    mvwprintw(output_win, 1, 1, "Chua co da thuc noi suy Lagrange");

                break;
            case 4:     /* Bình phương sai số */
                *haveF = FALSE;
                mvwprintw(output_win, 1, 1, "%s", choices[3]);
                nocbreak(); echo();
                mvwprintw(output_win, 2, 1, "Nhap bac xap xi can tim (> 0): ");
                wscanw(output_win, "%d", &deg);
                while (deg < 0)
                {
                    mvwprintw(output_win, 2, 1, "Nhap bac xap xi can tim (> 0): ");
                    wscanw(output_win, "%d", &deg);
                }
                noecho(); cbreak();
                *fullDeg = deg+1;

                wclear(output_win);
                box(output_win, 0, 0);
                mvwprintw(output_win, 1, 1, "%s voi bac = %d", choices[3], deg);
                fprintf(*fp,"\n\n>>> Phuong phap binh phuong sai so tim ham xap xi bac %d:\n", deg);
                wrefresh(output_win);

                getyx(output_win, y, x);
                mvwprintw(output_win, y+1, 1, "- Voi n la co cua bang so lieu, m la bac cua da thuc xap xi:");
                mvwprintw(output_win, y+2, 1, "    n.a0 + \u03A3xi.a1 + \u03A3xi^2.a2 + ... + \u03A3xi^m.am = \u03A3yi");
                mvwprintw(output_win, y+3, 1, "    \u03A3xi.a0 + \u03A3xi^2.a1 + \u03A3xi^3.a2 + ... + \u03A3xi^m+1.am = \u03A3yi.xi");
                mvwprintw(output_win, y+4, 1, "    ..............");
                mvwprintw(output_win, y+5, 1, "    \u03A3xi^m.a0 + \u03A3xi^(m+1).a1 + \u03A3xi^(m+2).a2 + ... + \u03A3xi^2m.am = \u03A3yi.xi^m");

                *F = lsq(output_win, X, Y, nInput, nDigit, deg, &(*fp));

                getyx(output_win, y, x);
                wmove(output_win, y+1, 1);
                wprintw(output_win, "===> F(x) = ");
                fprintf(*fp,"\n===> F(x) = ");
                printPoly(output_win, *F, deg+1, nDigit, &(*fp));

                getyx(input_win, y, x);
                wmove(input_win, y, 1);
                wclrtoeol(input_win);
                box(input_win, 0, 0);
                mvwprintw(input_win, y, 1, "Da thuc xap xi bac %d tim duoc: F(x) = ", deg);
                printPoly(input_win, *F, deg+1, nDigit, NULL);
                wrefresh(input_win);

                break;
            case 5:     /* Thoát và hiển thị */
                mvwprintw(output_win, 1, 1, "%s", choices[4]);
                return TRUE;
        };
        return FALSE;
    }
    else
        mvwprintw(output_win, 1, 1, "Chua co input");

    if (choice == 5)    /* Thoát khi chưa có gì để hiển thị */
        return TRUE;
    return FALSE;
}

void print_input(WINDOW *input_win, double X[], double Y[], int nInput, int nDigit, FILE **fp)
{
    int i, j;
    int x, y;

    fprintf(*fp, "\n=====================================INPUT=====================================\n");
    wclear(input_win);
    mvwprintw(input_win, 1, 1, "nInput = %d, nDigit = %d", nInput, nDigit);
    fprintf(*fp, "nInput = %d, nDigt = %d\n", nInput, nDigit);
    box(input_win, 0, 0);
    wrefresh(input_win);

    mvwprintw(input_win, 2, 1, "Bang so: { ");
    for (i = 0; i < nInput; i++)
    {
        getyx(input_win, y, x);
        if (x + 20 > INPUT_WIDTH)
        {
            if (y == INPUT_HEIGHT - 3)
            {
                wprintw(input_win, "(...) }");
                break;
            }
            else
                wmove(input_win, y+1, 12);
        }
        if (i != nInput - 1)
            wprintw(input_win, "(%.3lf, %.3lf), ", X[i], Y[i]);
        else
            wprintw(input_win, "(%.3lf, %.3lf) }", X[i], Y[i]);
    }
    // In bảng số ra file
    fprintf(*fp, "Bang so: { ");
    for (i = 0; i < 4; i++)
        fprintf(*fp, "(%.3lf, %.3lf), ", X[i], Y[i]);
    j = 0;
    for (i = 4; i < nInput; i++)
    {
        if (j%4 == 0)
        {
            fprintf(*fp, "\n           (%.3lf, %.3lf)", X[i], Y[i]);
            if (i == nInput -1)
                fprintf(*fp, " }");
            else
                fprintf(*fp, ", ");
        }
        else
        {
            fprintf(*fp, "(%.3lf, %.3lf)", X[i], Y[i]);
            if (i == nInput -1)
                fprintf(*fp, " }");
            else
                fprintf(*fp, ", ");
        }
        j += 1;
    }

    getyx(input_win, y, x);
    wmove(input_win, y+1, 1);
    wrefresh(input_win);
}

int nhapBangSoLieu(WINDOW *input_win, double X[], double Y[])
{
    int nInput = 0;
    int i, j;
    bool valid;

    // Nhập nInput
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "Co cua bang so lieu (n > 2 va n < 31): ");
    wscanw(input_win, "%d", &nInput);
    wrefresh(input_win);
    while (nInput <= 2 || nInput > 30)
    {
        wclear(input_win);
        box(input_win, 0, 0);
        wrefresh(input_win);
        mvwprintw(input_win, 1, 1, "Co cua bang so lieu (n > 2 va n < 31): ");
        wscanw(input_win, "%d", &nInput);
    }
    // Nhập bảng số liệu
    for (i = 0; i < nInput; i++)
    {
        valid = TRUE;

        wclear(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "nInput = %d", nInput);
        mvwprintw(input_win, 2, 1, "So lieu thu %d:", i+1);
        wrefresh(input_win);

        mvwprintw(input_win, 3, 1, "X = ");
        while (wscanw(input_win, "%lf", &X[i]) != 1)
        {
            wmove(input_win, 3, 1);
            wclrtoeol(input_win);
            box(input_win, 0, 0);
            mvwprintw(input_win, 3, 1, "X = ");
            wrefresh(input_win);
        }

        for (j =  0; j < i; j++)
        {
            if (X[j] == X[i])
            {
                valid = FALSE;
                break;
            }
        }

        if (valid == FALSE)
        {
            --i;
            mvwprintw(input_win, 4, 1, "%lf da co trong bang so lieu, xin nhap lai so khac", X[j]);
            wrefresh(input_win);
            wgetch(input_win);
            continue;
        }
        mvwprintw(input_win, 4, 1, "Y = ");
        while (wscanw(input_win, "%lf", &Y[i]) != 1)
        {
            wmove(input_win, 4, 1);
            wclrtoeol(input_win);
            box(input_win, 0, 0);
            mvwprintw(input_win, 4, 1, "Y = ");
            wrefresh(input_win);
        }
    }
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "Hoan tat nhap!");
    wrefresh(input_win);

    // Nhận input ko cần Enter và không hiện input nhập từ bàn phím lên màn hình
    noecho(); cbreak();
    wgetch(input_win);

    return nInput;
}

void print_precision(WINDOW *win, double num, int nDigit, FILE **fp)
{
    int i;
    long int current_digit;
    long int digits[nDigit+1];
    unsigned char rounded_to_0 = 1;

    // Lấy số sau dấu phẩy (chưa làm tròn)
    for (i = 0; i < nDigit+1; i++)
    {
        current_digit = (long int)(num * pow(10, i+1)) % 10;
        digits[i] = fabs(current_digit);
    }

    // Làm tròn số
    for (i = nDigit-1; i >= 0; i--)
    {
        if (digits[i+1] >= 5 || (i < nDigit-1 && digits[i+1] == 0) )
        {
            if (digits[i] == 9)
            {
                digits[i] = 0;
                continue;
            }
            digits[i] += 1;
        }
        rounded_to_0 = 0; // Số đầu tiên sau dấu phẩy không bị làm tròn về 0
        break;
    }

    // In kết quả làm tròn ra màn hình
    if (rounded_to_0)
    {
        num > 0 ? wprintw(win, "%d.", (int)num + 1) : wprintw(win, "-%d.", (int)-num - 1) ;
        if (&(*fp) != NULL)
            num > 0 ? fprintf(*fp, "%d.", (int)num + 1) : fprintf(*fp, "-%d.", (int)-num - 1) ;
    }
    else
    {
        num > 0 ? wprintw(win, "%d.", (int)num) : wprintw(win, "-%d.", (int)-num);
        if (&(*fp) != NULL)
            num > 0 ? fprintf(*fp, "%d.", (int)num) : fprintf(*fp, "-%d.", (int)-num);
    }

    for (i = 0; i < nDigit; i++)
    {
        wprintw(win, "%ld", digits[i]);
        if (&(*fp) != NULL)
            fprintf(*fp, "%ld", digits[i]);
    }
}

void printPoly(WINDOW* win, double poly[], int n, int nDigit, FILE **fp)
{
    int i;

    for (i = n-1; i >= 0; i--)
    {
        if (fabs(poly[i]) >  10e-10) // Chỉ in khi hệ số khác 0
        {
            // In hệ số
            if (poly[i] != 1 || i == 0)
            {
                if (i != n-1)
                    print_precision(win, poly[i] > 0 ? poly[i] : -poly[i], nDigit, &(*fp));
                else
                    print_precision(win, poly[i], nDigit, &(*fp));
            }
            // In biến số
            if (i != 0)
            {
                i > 1 ? wprintw(win, "x^%d", i) : wprintw(win, "x");
                if (&(*fp) != NULL)
                    i > 1 ? fprintf(*fp, "x^%d", i) : fprintf(*fp, "x");
            }
            // In dấu '+', '-'
            if (i != 0 && fabs(poly[i-1]) > 10e-10)
            {
                poly[i-1] > 0 ? wprintw(win, " + ") : wprintw(win, " - ");
                if (&(*fp) != NULL)
                    poly[i-1] > 0 ? fprintf(*fp, " + ") : fprintf(*fp, " - ");
            }
        }
    }
}

// Nội suy Lagrange
double *multiPoly(double A[], double B[], int m, int n)
{
    double *prod = malloc (sizeof (double) * (m+n-1));

    for (int i = 0; i < m+n-1; i++)
        prod[i] = 0;

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            prod[i+j] += A[i]*B[j];

    return prod;
}

double *lagrange(WINDOW *win, double X[], double Y[], double L[30][30], int nInput, int nDigit, FILE **fp)
{
    double *F = malloc (sizeof (double) * nInput);
    double poly[nInput][2];       // Các đa thức con cần dùng trong quá trình tính toán
    int x, y;
    // Biến chạy vòng lặp for
    int i, j, k;        
    // Biến tạm trong quá trình tính toán
    double *tempPoly;
    double temp;

    // Khởi tạo các giá trị của F
    for (i = 0; i < nInput; i++)
        F[i] = 0;

    /* 1. Tim các đa thức Lagrange cơ bản và in ra màn hình */

    /* Khởi tạo các đa thức con từ mảng X */
    for (i = 0; i < nInput; i++)
    {
        poly[i][0] = -X[i];
        poly[i][1] = 1;
    }
    for (i = 0; i < nInput; i++)
    {
        getyx(win, y, x);
        wmove(win, y+1, 1);
        /* Khởi tạo tempPoly để tính đa thức Lagrange cơ bản */
        if (i == 0)
            tempPoly = poly[1];
        else
            tempPoly = poly[0];

        /* Tính đa thức Lagrange cơ bản L[i] */

        // Tìm tử số của L[i] lưu vào biến tạm tempPoly
        k = 0;
        for (j = 0; j < nInput; j++)
        {
            if ((j != 0 && j != i && i != 0) || (i == 0 && j > 1))
            {
                tempPoly = multiPoly(tempPoly, poly[j], k+2, 2);
                k++;
            }
        }
        
        // Tính mẫu số của L[i] lưu vào biến tạm temp
        temp = 1;
        for (j = 0; j < nInput; j++)
        {
            if (j != i)
                temp *= -poly[i][0] + poly[j][0];
        }

        /* Tính rồi lưu đa thức Lagrange cơ bản L[i] vào mảng L */
        for (j = 0; j < nInput; j++)
            L[i][j] = tempPoly[j] / temp;

        // In đa thức cơ bản vừa tìm được ra màn hình 
        wprintw(win, "L%d(x) = ", i + 1);
        fprintf(*fp, "L%d(x) = ", i + 1);
        printPoly(win, L[i], nInput, nDigit, &(*fp));
        fprintf(*fp, "\n");
        wrefresh(win);
    }
    /* Giải phóng bộ nhớ cho tempPoly sau khi không còn sử dụng */
    free(tempPoly);

    /* 2, Tìm các hệ số của đa thức nội suy thông qua các đa thức cơ bản */
    for (i = 0; i < nInput; i++)
    {
        for (j = 0; j < nInput; j++)
        {
            F[i] += L[j][i] * Y[j];
        }
    }
    return F;
}

// Bình phương sai số
double *gauss(double *matrix, int n)
{
    int i, j, k, l;
    double *ans;
    double M[n][n+1];
    unsigned found_non_zero;
    double temp;

    for (i = 0; i < n; i++)
        for (j = 0; j < n + 1; j++)
            M[i][j] = *((matrix + i*n) + j + i);

    /* Giải hệ n phương trình, n ẩn */
    i = j = 0;
    while ((i < n) && (j < n+1))
    {
        // Tìm vị trí bằng 0 đầu tiên khác không của mỗi hàng
        if (M[i][j] == 0)
        {
            found_non_zero = 0;
            for (k = i+1; k < n; k++)
            {
                if (M[k][j] != 0)
                {
                    found_non_zero = 1;
                    // Đổi hàng k với i
                    for (l = 0; l < n+1; l++)
                    {
                        temp = M[k][l];
                        M[k][l] = M[i][l];
                        M[i][l] = temp;
                    }
                    break;
                }
            }
            if (found_non_zero == 0)
            {
                j++;
                continue;
            }
        }
        // Khử các vị trí dưới
        for (k = i+1; k < n; k++)
        {
            temp = M[k][j] / M[i][j];
            for (l = 0; l < n+1; l++)
                M[k][l] = M[k][l] - temp * M[i][l];
        }
        i++;
        j++;
    }
    // Khử các vị trí trên
    for (j = n-1; j > 0; j--)  // n = 3
    {
        for (i = j-1; i >= 0; i--)
        {
            temp = M[i][j] / M[j][j];
            for (k = j-1; k < n+1; k++)
                M[i][k] = M[i][k] - temp * M[j][k];
        }
    }

    // Gán các kết quả vào mảng ans
    ans = (double*) malloc(sizeof(double) * (n-1));
    for (i = 0; i < n; i++)
        ans[i] = M[i][n] / M[i][i];

    return ans;
}

double *lsq(WINDOW *win, double X[], double Y[], int nInput, int nDigit, int deg, FILE **fp)
{
    int i, j;
    int nFactor = 2*deg + 1;
    int degA = deg + 1;
    double A[degA][degA];
    double b[degA];
    double factor[nFactor];
    double matrix[deg][deg+2];
    double *F;
    int y, x;

    /* Tính các hệ số ma trận vuông cấp deg */
    getyx(win, y, x);
    wmove(win, y+2, 1);
    wprintw(win, "- Tinh cac he so cua he phuong trinh");
    fprintf(*fp, "- Tinh cac he so cua he phuong trinh\n");

    for (i = 0; i < nFactor; i++)
    {
        factor[i] = 0;
        b[i] = 0;
        for (j = 0; j < nInput; j++)
        {
            factor[i] += pow(X[j], i);
            if (i < degA)
                b[i] += Y[j] * pow(X[j], i);
        }
    }

    // In kết quả tính hệ số ra màn hình
    getyx(win, y, x);
    mvwprintw(win, y+1, 1, "\u03A3xi = ");
    fprintf(*fp, "\u03A3xi = ");
    print_precision(win, factor[1], nDigit, &(*fp));
    fprintf(*fp, "\n");

    mvwprintw(win, y+2, 1, "\u03A3yi = ");
    fprintf(*fp, "\u03A3yi = ");
    print_precision(win, b[0], nDigit, &(*fp));
    fprintf(*fp, "\n");

    for (i = 1; i < nFactor-1; i++)
    {
        getyx(win, y, x);
        mvwprintw(win, y+1, 1, "\u03A3xi^%d = ", i+1);
        fprintf(*fp, "\u03A3xi^%d = ", i+1);
        print_precision(win, factor[i+1], nDigit, &(*fp));
        fprintf(*fp, "\n");
    }

    for (j = 1; j < degA; j++)
    {
        getyx(win, y, x);
        if (j == 1)
        {
            mvwprintw(win, y+1, 1, "\u03A3xi.yi = ");
            fprintf(*fp, "\u03A3xi.yi = ");
            print_precision(win, b[1], nDigit, &(*fp));
            fprintf(*fp, "\n");
        }
        else
        {
            mvwprintw(win, y+1, 1, "\u03A3xi^%d.yi = ", j);
            fprintf(*fp, "\u03A3xi^%d.yi = ", j);
            print_precision(win, b[j], nDigit, &(*fp));
            fprintf(*fp, "\n");
        }
    }
    wrefresh(win);

    // Gán các hệ số vào mảng 2 chiều biểu diễn ma trận
    for (i = 0; i < degA; i++)
        for (j = 0; j < degA; j++)
            A[i][j] = factor[i+j];

    // Tìm ma trận đẻ giải hệ phương trình rút ra Đa thức nội suy
    for (i = 0; i < degA; i++)
    {
        for (j = 0; j < degA; j++)
            matrix[i][j] = A[i][j];
        matrix[i][degA] = b[i];
    }

    // In ra mản hình Hệ phương trình tìm được
    getyx(win, y, x);
    wmove(win, y+2, 1);
    wprintw(win, "- He phuong trinh thu duoc tu bang so lieu:");
    fprintf(*fp, "\n- He phuong trinh thu duoc tu bang so lieu:\n");
    for (i = 0; i < degA; i++)
    {
        getyx(win, y, x);
        wmove(win, y+1, 1);
        wprintw(win, "    ");
        for (j = 0; j < degA; j++)
        {
            print_precision(win, matrix[i][j], nDigit, &(*fp));
            wprintw(win, "*a%d %c ", j+1, j == degA - 1 ? '\b' : '+');
            if (j != degA - 1)
                fprintf(*fp, "*a%d + ", j+1);
            else
                fprintf(*fp, "*a%d ", j+1);
        }
        wprintw(win, "= ");
        fprintf(*fp, "= ");
        print_precision(win, matrix[i][degA], nDigit, &(*fp));
        fprintf(*fp, "\n");
    }

    getyx(win, y, x);
    wmove(win, y+2, 1);
    wprintw(win, "- Giai he tren thu duoc:");
    fprintf(*fp, "\n- Giai he tren thu duoc:\n");
    wmove(win, y+3, 1);
    /* Giải hệ phương trình ma trận 'matrix' để tính các hệ số của đa thức nội suy */
    F = gauss((double*)matrix, degA);
    for (i = 0; i < degA; i++)
    {
        wprintw(win, "a%d = ", i+1);
        fprintf(*fp, "a%d = ", i+1);
        print_precision(win, F[i], nDigit, &(*fp));
        fprintf(*fp, "\n");
        getyx(win, y, x);
        wmove(win, y+1, 1);
    }

    wrefresh(win);
    return F;
}

// Horner
double horner(WINDOW *win, double F[], double x0, int fullDeg, int nDigit, FILE **fp)
{
    int i;
    double result;
    int deg = fullDeg;
    int y, x;
    double temp;

    for (i = fullDeg-1; i >= 0; i--)
    {
        if(fabs(F[i]) < 10e-10)
            deg--;
        else
            break;
    }

    // In lược đồ horner
    getyx(win, y, x);
    mvwprintw(win, y, 1, "%.3lf |", x0);
    fprintf(*fp, "\n%8.3lf |", x0);
    getyx(win, y, x);
    for (i = deg -1; i >= 0; i--)
    {
        wprintw(win, "%8.3lf", F[i]);
        fprintf(*fp, "%8.3lf", F[i]);
    }
    fprintf(*fp, "\n");
    wattron(win, A_UNDERLINE);
    mvwprintw(win, y+1, x-1, "|%8.3lf", 0);
    fprintf(*fp, "%10c%8.3lf", '|', 0.0);
    temp = F[deg -1];
    for (i = deg -2; i >= 0; i--)
    {
        wprintw(win, "%8.3lf", x0*temp);
        fprintf(*fp, "%8.3lf", x0*temp);
        temp = F[i] + x0*temp;
    }
    fprintf(*fp, "\n");
    fprintf(*fp, "         --------------------------------------------------\n");
    wattroff(win, A_UNDERLINE);
    mvwprintw(win, y+2, x-1, "|%8.3lf", F[deg-1]);
    fprintf(*fp, "%10c%8.3lf", '|', F[deg-1]);
    temp = F[deg-1];
    for (i = deg -2; i>= 0; i--)
    {
        wprintw(win, "%8.3lf", F[i] + x0*temp);
        fprintf(*fp, "%8.3lf", F[i] + x0*temp);
        temp = F[i] + x0*temp;
    }
    fprintf(*fp, "\n");

    // In quá trình tính cụ thể
    result = 0;
    getyx(win, y, x);
    mvwprintw(win, y+2, 1, "khoi tao result = 0");
    fprintf(*fp, "\nkhoi tao result = 0");
    for (i = deg-1; i >= 0; i--)
    {
        getyx(win, y, x);
        wmove(win, y+1, 1);
        fprintf(*fp, "\n");
        wprintw(win, "%d. result = result * ", deg - i);
        fprintf(*fp, "%d. result = result * ", deg - i);
        print_precision(win, x0, nDigit, &(*fp));
        fprintf(*fp, " %c ", F[i] > 0 ? '+' : '-');
        print_precision(win, F[i] > 0 ? F[i] : -F[i], nDigit, &(*fp));
        result = result * x0 + F[i];
        wprintw(win, "    --> %s result = ", i != 0 ? "" : " final");
        fprintf(*fp, "    --> %s result = ", i != 0 ? "" : " final");
        print_precision(win, result, nDigit, &(*fp));
    }

    return result;
}
