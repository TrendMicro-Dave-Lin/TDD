#include "gtest/gtest.h"
#include "gmock/gmock.h"

struct Budget
{
    char YearMonth[7];
    int Amount;
};

class BudgetRepo
{
public:
    BudgetRepo() {}
    BudgetRepo(std::vector<Budget> budgets) : budgets(budgets) {}
    ~BudgetRepo() {}
    std::vector<Budget> getAll() {
        return budgets;
    };
    std::vector<Budget> budgets;
};

class BudgetService
{
public:
    BudgetService(BudgetRepo _repo)
    {
        repo = _repo;
    }
    double query(std::tm start, std::tm end)
    {
        double ret = 0.;
        int daysDiff = 0;
        double oneDayAmount = 0.;
        if (sameYear(start, end)) {
            if (sameMonth(start, end)) {
                oneDayAmount = getOneDayAmount(start);
                daysDiff = getDaysDiff(start, end);               
            }
            else {
                for (int month = start.tm_mon; month <= end.tm_mon; ++month) {
                    if (month == start.tm_mon) {
                        tm endOfStartMonth = start;
                        endOfStartMonth.tm_mday = getDaysOfMonth(start.tm_year, start.tm_mon);
                        ret += query(start, endOfStartMonth);
                    }
                    else if (month == end.tm_mon) {
                        tm startOfEndMonth = end;
                        startOfEndMonth.tm_mday = 1;
                        ret += query(startOfEndMonth, end);
                    }
                    else {
                        tm monthStart, monthEnd;
                        monthStart.tm_year = start.tm_year;
                        monthStart.tm_mon = month;
                        monthStart.tm_mday = 1;
                        monthEnd.tm_year = start.tm_year;
                        monthEnd.tm_mon = month;
                        monthEnd.tm_mday = getDaysOfMonth(start.tm_year, month);;
                        ret += query(monthStart, monthEnd);

                    }
                }
            }
        }

        ret += oneDayAmount * daysDiff;
        std::cout << ret << std::endl;
        
        return ret;
    }

private:
    BudgetRepo repo;
    double getOneDayAmount(std::tm date)
    {
        std::string yearMonth = std::to_string(date.tm_year);
        yearMonth += date.tm_mon >= 10 ? "" : "0";
        yearMonth += std::to_string(date.tm_mon);
        std::vector<Budget> budgets = repo.getAll();
        for (auto budget : budgets) {
            std::string budgetYearMonth = budget.YearMonth;
            if (budgetYearMonth == yearMonth) {
                return budget.Amount / getDaysOfMonth(date.tm_year, date.tm_mon);
            }
        }
        return 0.;
    }
    bool sameYear(std::tm start, std::tm end)
    {
        return start.tm_year == end.tm_year;
    }
    bool sameMonth(std::tm start, std::tm end)
    {
        return start.tm_mon == end.tm_mon;
    }

    // 計算兩個日期相差天數
    // 已知年月，計算出該月天數
    int getDaysOfMonth(int year, int month)
    {
        int days = 0;
        if (month == 2) {
            if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
                days = 29;
            }
            else {
                days = 28;
            }
        }
        else if (month == 4 || month == 6 || month == 9 || month == 11) {
            days = 30;
        }
        else {
            days = 31;
        }
        return days;
    }

    // 計算兩個日期相差天數
    int getDaysDiff(std::tm start, std::tm end)
    {
        int daysDiff = 0;
        if (start.tm_mon == end.tm_mon) {
            daysDiff = end.tm_mday - start.tm_mday + 1;
        }
        else {
            int daysInStartMonth = getDaysOfMonth(start.tm_year, start.tm_mon);
            daysDiff = daysInStartMonth - start.tm_mday + 1;
            for (int month = start.tm_mon + 1; month < end.tm_mon; ++month) {
                daysDiff += getDaysOfMonth(start.tm_year, month);
            }
            daysDiff += end.tm_mday;
        }
        return daysDiff;
    }

    //int getDaysDiff(std::tm start, std::tm end)
    //{
    //    time_t time1 = std::mktime(&start);
    //    time_t time2 = std::mktime(&end);

    //    // 计算秒数差    
    //    double secondsDiff = difftime(time2, time1);

    //    // 转换秒数差为天数差    
    //    int daysDiff = (static_cast<int>(secondsDiff / (60 * 60 * 24))) + 1;

    //    std::cout << "日期相差天数: " << daysDiff << " 天" << std::endl;
    //    return daysDiff;
    //}
};


tm createTM(int year, int month, int day)
{
    tm ret;
    ret.tm_year = year;
    ret.tm_mon = month;
    ret.tm_mday = day;
    return ret;
}
TEST(BudgetService, Oneday)
{   
    tm start = createTM(2023, 11, 1);
    tm end = createTM(2023, 11, 1);
    BudgetRepo repo({ {"202311", 3000} });
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 100.);
}


TEST(BudgetService, Twodays)
{
    tm start = createTM(2023, 4, 1);
    tm end = createTM(2023, 4, 2);
    BudgetRepo repo({ {"202304", 3000}, {"202305", 3100}});
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 200.);
}

TEST(BudgetService, OneMonth)
{
    tm start = createTM(2023, 4, 1);
    tm end = createTM(2023, 4, 30);
    BudgetRepo repo({ {"202304", 3000}, {"202305", 3100} });
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 3000.);
}

TEST(BudgetService, NullBudget)
{
    tm start = createTM(2023, 4, 1);
    tm end = createTM(2023, 4, 30);
    BudgetRepo repo({ {"202305", 3100} });
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 0.);
}

TEST(BudgetService, Accross2Months)
{
    tm start = createTM(2023, 4, 1);
    tm end = createTM(2023, 5, 30);
    BudgetRepo repo({ {"202304", 3000}, {"202305", 3100} });
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 6000.);
}

TEST(BudgetService, Accross3Months)
{
    tm start = createTM(2023, 4, 1);
    tm end = createTM(2023, 6, 30);
    BudgetRepo repo({ {"202304", 3000}, {"202305", 3100}, {"202306", 3000} });
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 9100.);
}

TEST(BudgetService, AccrossYears)
{
    tm start = createTM(2023, 12, 1);
    tm end = createTM(2024, 1, 30);
    BudgetRepo repo({ {"202312", 3000}, {"202401", 3100} });
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 6000.);
}