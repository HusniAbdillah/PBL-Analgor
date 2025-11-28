#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <algorithm>
using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define BG_GREEN "\033[42m"
#define BG_RED   "\033[41m"
#define BG_BLUE  "\033[44m"
#define BG_YELLOW "\033[43m"

struct Event {
    int start, end;
    string name;
    bool selected = false;
    int step_selected = -1;
};

int parseTimeAt(const string& line, int pos) {
    int hour = (line[pos] - '0') * 10 + (line[pos + 1] - '0');
    int min = (line[pos + 3] - '0') * 10 + (line[pos + 4] - '0');
    if (hour == 24 && min == 0) return 1440;
    return hour * 60 + min;
}

string minToTime(int minutes) {
    if (minutes == 1440) return "24:00";
    int hour = minutes / 60, min = minutes % 60;
    char result[6];
    result[0] = '0' + hour / 10;
    result[1] = '0' + hour % 10;
    result[2] = ':';
    result[3] = '0' + min / 10;
    result[4] = '0' + min % 10;
    result[5] = '\0';
    return string(result);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    cout << "\033[H";
}

void hideCursor() {
    cout << "\033[?25l";
}

void showCursor() {
    cout << "\033[?25h";
}

void moveCursor(int row, int col) {
    cout << "\033[" << row << ";" << col << "H";
}

void printHeader() {
    cout << BOLD << CYAN;
    cout << "+==============================================================================+\n";
    cout << "|                     * VISUALISASI ALGORITMA GREEDY *                        |\n";
    cout << "|                    >> PENJADWALAN GEDUNG BALAI DESA <<                      |\n";
    cout << "+==============================================================================+" << RESET << endl;
}

void printTimeline(const vector<Event>& all_events, int current_step, int last_end) {
    cout << "\n" << BOLD << BG_BLUE << " TIMELINE PENJADWALAN (24 JAM) " << RESET << endl;
    
    cout << "\n" << CYAN << "Waktu: ";
    for (int h = 0; h < 24; h += 2) {
        cout << setw(3) << setfill('0') << h << "   ";
    }
    cout << RESET << "\n" << CYAN << "       ";
    for (int h = 0; h < 24; h += 2) {
        cout << "|    ";
    }
    cout << RESET << endl;
    
    if (last_end > 0) {
        cout << "\n" << BG_YELLOW << " Aktivitas terakhir selesai pukul: " << minToTime(last_end) << " " << RESET << endl;
    }
    
    cout << "\n" << BOLD << "DAFTAR KEGIATAN:" << RESET << endl;
    cout << string(80, '-') << endl;
    
    for (int i = 0; i < all_events.size(); i++) {
        const auto& event = all_events[i];
        
        string color = WHITE;
        string status = "MENUNGGU";
        string icon = "[O]";
        
        if (event.selected) {
            color = GREEN;
            status = "TERPILIH";
            icon = "[+]";
        } else if (i < current_step) {
            color = RED;
            status = "DITOLAK";
            icon = "[-]";
        } else if (i == current_step) {
            color = YELLOW;
            status = "SEDANG DIPROSES";
            icon = "[*]";
        }
        
        cout << color << icon << " " << minToTime(event.start) << "-" << minToTime(event.end)
             << " [" << status << "] " << event.name << RESET << endl;
        
        cout << "     ";
        for (int h = 0; h < 24; h++) {
            int hour_start = h * 60;
            int hour_end = (h + 1) * 60;
            
            if (event.start < hour_end && event.end > hour_start) {
                if (event.selected) {
                    cout << GREEN << "##" << RESET;
                } else if (i < current_step) {
                    cout << RED << "xx" << RESET;
                } else if (i == current_step) {
                    cout << YELLOW << ".." << RESET;
                } else {
                    cout << WHITE << "--" << RESET;
                }
            } else {
                cout << "  ";
            }
            cout << " ";
        }
        cout << endl;
        if (i < all_events.size() - 1) cout << endl;
    }
    cout << string(80, '-') << endl;
}

void printBucketStatus(const vector<vector<Event>>& bucket, int current_time, int step) {
    cout << "\n" << BOLD << BLUE << "BUCKET STATUS" << RESET << endl;
    cout << "Sedang memproses waktu: " << YELLOW << minToTime(current_time) << RESET << endl;
    
    if (!bucket[current_time].empty()) {
        cout << "Events di bucket[" << current_time << "]:" << endl;
        for (const auto& event : bucket[current_time]) {
            cout << "  - " << event.name << " (" << minToTime(event.start) 
                 << "-" << minToTime(event.end) << ")" << endl;
        }
    } else {
        cout << "Tidak ada event di waktu ini." << endl;
    }
}

void printAlgorithmStep(const Event& event, int step, int last_end, bool will_select) {
    cout << "\n" << BOLD << ">> STEP " << step << RESET << endl;
    cout << "================================================" << endl;
    
    cout << "Mengecek kegiatan: " << CYAN << event.name << RESET << endl;
    cout << "Waktu: " << minToTime(event.start) << " - " << minToTime(event.end) << endl;
    cout << "Kondisi: event.start (" << minToTime(event.start) << ") >= last_end (" 
         << minToTime(last_end) << ") ?" << endl;
    
    if (will_select) {
        cout << GREEN << "KEPUTUSAN: DIPILIH!" << RESET << endl;
        cout << "   Alasan: " << minToTime(event.start) << " >= " << minToTime(last_end) 
             << " (TIDAK OVERLAP)" << endl;
        cout << "   Update: last_end = " << minToTime(event.end) << endl;
    } else {
        cout << RED << "KEPUTUSAN: DITOLAK!" << RESET << endl;
        cout << "   Alasan: " << minToTime(event.start) << " < " << minToTime(last_end) 
             << " (ADA OVERLAP)" << endl;
    }
}

void printFinalResult(const vector<Event>& result, const vector<Event>& all_events) {
    cout << "\n" << BOLD << BG_GREEN << " ** HASIL AKHIR PENJADWALAN BALAI DESA ** " << RESET << endl;
    cout << string(80, '=') << endl;
    
    cout << BOLD << "STATISTIK KESELURUHAN:" << RESET << endl;
    cout << "|- Total kegiatan yang diajukan: " << CYAN << all_events.size() << RESET << " kegiatan" << endl;
    cout << "|- Total kegiatan terpilih: " << GREEN << result.size() << RESET << " kegiatan" << endl;
    
    double selection_rate = result.empty() ? 0.0 : (double)result.size() / (double)all_events.size() * 100.0;
    cout << "\\- Tingkat penerimaan: " << YELLOW << fixed << setprecision(1) 
         << selection_rate << "%" << RESET << endl;
    
    if (result.empty()) {
        cout << "\n" << RED << "TIDAK ADA KEGIATAN YANG DAPAT DIJADWALKAN" << RESET << endl;
        return;
    }
    
    cout << "\n" << BOLD << ">> JADWAL FINAL BALAI DESA:" << RESET << endl;
    cout << string(80, '-') << endl;
    
    vector<Event> sorted_result = result;
    sort(sorted_result.begin(), sorted_result.end(), [](const Event& a, const Event& b) {
        return a.start < b.start;
    });
    
    for (int i = 0; i < sorted_result.size(); i++) {
        const auto& event = sorted_result[i];
        int duration = event.end - event.start;
        cout << GREEN << "[+] [" << setw(2) << i+1 << "] " 
             << minToTime(event.start) << " - " << minToTime(event.end)
             << " (" << setw(3) << duration << " menit) -> " << event.name << RESET << endl;
    }
    
    int total_used_minutes = 0;
    for (const auto& event : result) {
        total_used_minutes += (event.end - event.start);
    }
    
    cout << "\n" << BOLD << ">> ANALISIS UTILISASI GEDUNG:" << RESET << endl;
    cout << string(80, '-') << endl;
    
    double utilization = (double)result.size() / (double)all_events.size() * 100.0;
    string util_color = utilization >= 70 ? GREEN : utilization >= 40 ? YELLOW : RED;
    cout << "Tingkat utilisasi gedung: " << util_color << BOLD << fixed << setprecision(1) 
         << utilization << "%" << RESET;
    
    if (utilization >= 70) {
        cout << " " << GREEN << "(OPTIMAL)" << RESET << endl;
    } else if (utilization >= 40) {
        cout << " " << YELLOW << "(CUKUP BAIK)" << RESET << endl;
    } else {
        cout << " " << RED << "(PERLU OPTIMASI)" << RESET << endl;
    }
    
    cout << "(Berdasarkan jumlah kegiatan yang dapat diakomodasi: " << result.size() 
         << " dari " << all_events.size() << " kegiatan)" << endl;
    
    cout << "\n" << BOLD << ">> ANALISIS WAKTU KOSONG:" << RESET << endl;
    cout << string(80, '-') << endl;
    
    int total_gap = 0;
    vector<pair<int, int>> gaps;
    
    if (sorted_result[0].start > 0) {
        gaps.push_back({0, sorted_result[0].start});
        total_gap += sorted_result[0].start;
    }
    
    for (int i = 0; i < sorted_result.size() - 1; i++) {
        int gap_start = sorted_result[i].end;
        int gap_end = sorted_result[i + 1].start;
        if (gap_end > gap_start) {
            gaps.push_back({gap_start, gap_end});
            total_gap += (gap_end - gap_start);
        }
    }
    
    if (sorted_result.back().end < 1440) {
        gaps.push_back({sorted_result.back().end, 1440});
        total_gap += (1440 - sorted_result.back().end);
    }
    
    if (gaps.empty()) {
        cout << GREEN << "[+] TIDAK ADA WAKTU KOSONG - PENJADWALAN OPTIMAL!" << RESET << endl;
    } else {
        cout << "Periode waktu kosong:" << endl;
        for (const auto& gap : gaps) {
            int gap_minutes = gap.second - gap.first;
            cout << "  [-] " << minToTime(gap.first) << " - " << minToTime(gap.second) 
                 << " (" << gap_minutes << " menit)" << endl;
        }
        cout << "\nTotal waktu kosong: " << RED << total_gap << " menit" << RESET << endl;
        cout << "Persentase waktu tidak terpakai: " << RED << fixed << setprecision(1) 
             << (double)total_gap / 1440.0 * 100.0 << "%" << RESET << endl;
    }
    
    cout << string(80, '=') << endl;
}

void pauseForInput(const string& message = "Tekan ENTER untuk melanjutkan...") {
    cout << "\n" << string(50, '-') << endl;
    cout << BOLD << CYAN << ">> " << message << " <<" << RESET << endl;
    cout << string(50, '-') << endl;
    showCursor();
    cin.get();
    hideCursor();
    clearScreen();
}

void animateText(const string& text, int delay_ms = 50) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
    cout << endl;
}

void sleepMs(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

signed main() {
    int n; cin >> n;
    cin.ignore();
    
    vector<vector<Event>> bucket(1441);
    vector<Event> all_events;
    
    for (int i = 0; i < n; i++) {
        string line;
        
        if (!getline(cin, line)) {
            if (cin.eof() && !line.empty()) {
            } else {
                break;
            }
        }
        
        if (line.empty()) {
            i--; continue;
        }
        
        if (line.length() < 11) {
            cout << "Format tidak valid: baris terlalu pendek" << endl;
            continue;
        }
        
        int pos1 = -1, pos2 = -1;
        for (int j = 0; j < line.length(); j++) {
            if (line[j] == ' ') {
                if (pos1 == -1) pos1 = j;
                else if (pos2 == -1) {
                    pos2 = j;
                    break;
                }
            }
        }
        
        if (pos1 == -1 || pos2 == -1) {
            cout << "Format tidak valid: spasi tidak mencukupi" << endl;
            continue;
        }
        
        if (pos1 < 5 || pos2 < pos1 + 6) {
            cout << "Format tidak valid: format waktu salah" << endl;
            continue;
        }
        
        int start = parseTimeAt(line, 0);
        int end = parseTimeAt(line, pos1 + 1);
        
        string name = "";
        name.reserve(line.length() - pos2 - 1);
        for (int j = pos2 + 1; j < line.length(); j++) {
            name += line[j];
        }
        
        if (start >= end) {
            cout << "Kegiatan tidak valid: '" << name << "' (waktu mulai >= waktu selesai)" << endl;
            continue;
        }
        
        Event event = {start, end, name, false, -1};
        bucket[end].emplace_back(event);
        all_events.push_back(event);
    }
    
    hideCursor();
    clearScreen();
    printHeader();
    
    cout << "\n" << BOLD << BG_BLUE << " ALGORITMA GREEDY SCHEDULING " << RESET << endl;
    cout << "\n" << CYAN << "Strategi yang digunakan:" << RESET << endl;
    cout << "   * Earliest End Time First (Waktu selesai paling awal)" << endl;
    cout << "   * Bucket Sort untuk optimasi O(N)" << endl;
    cout << "   * Greedy Choice untuk solusi optimal" << endl;
    
    cout << "\n" << YELLOW << "Data input:" << RESET << endl;
    cout << "   * Total kegiatan: " << all_events.size() << endl;
    cout << "   * Domain waktu: 00:00 - 24:00 (1440 menit)" << endl;
    
    pauseForInput("Tekan ENTER untuk memulai simulasi algoritma...");
    
    vector<Event> result;
    result.reserve(n);
    int last = 0;
    int step = 1;
    int event_index = 0;
    
    for (int time = 0; time <= 1440; ++time) {
        if (bucket[time].empty()) continue;
        
        printHeader();
        printBucketStatus(bucket, time, step);
        pauseForInput("Tekan ENTER untuk memproses bucket ini...");
        
        for (auto& event : bucket[time]) {
            printHeader();
            
            bool will_select = (event.start >= last);
            
            printTimeline(all_events, event_index, last);
            printAlgorithmStep(event, step, last, will_select);
            
            if (will_select) {
                result.push_back(event);
                last = event.end;
                
                for (auto& ae : all_events) {
                    if (ae.name == event.name && ae.start == event.start) {
                        ae.selected = true;
                        ae.step_selected = step;
                        break;
                    }
                }
                
                cout << "\n" << GREEN << "KEGIATAN BERHASIL DITAMBAHKAN KE JADWAL!" << RESET << endl;
            } else {
                cout << "\n" << RED << "KEGIATAN DILEWATI KARENA KONFLIK WAKTU" << RESET << endl;
            }
            
            step++;
            event_index++;
            
            pauseForInput("Tekan ENTER untuk melanjutkan ke kegiatan berikutnya...");
        }
    }
    
    printHeader();
    
    cout << "\n" << BG_GREEN << " ALGORITMA GREEDY TELAH SELESAI DIEKSEKUSI! " << RESET << endl;
    animateText("\nMenampilkan hasil akhir...", 30);
    
    printTimeline(all_events, all_events.size(), last);
    printFinalResult(result, all_events);
    
    showCursor();
    
    return 0;
}