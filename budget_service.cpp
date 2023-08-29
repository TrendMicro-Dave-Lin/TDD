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
                oneDayAmount = getOneDayAmount((std::to_string(start.tm_year) + std::to_string(start.tm_mon)).c_str());
                daysDiff = getDaysDiff(start, end);               
            }
            /*else {
                for (int month = start.tm_mon; month <= end.tm_mon; ++month) {
                    if (month == start.tm_mon) {

                    }
                    else if (month == end.tm_mon) {

                    }
                    else {

                    }
                }
            }*/
        }

        ret = oneDayAmount * daysDiff;
        std::cout << ret << std::endl;
        
        return ret;
    }

private:
    BudgetRepo repo;
    double getOneDayAmount(const char* yearMonth)
    {
        std::vector<Budget> budgets = repo.getAll();
        for (auto budget : budgets) {
            if (strcmp(budget.YearMonth, yearMonth) == 0) {
                return budget.Amount / 30;
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

    int getDaysDiff(std::tm start, std::tm end)
    {
        time_t time1 = std::mktime(&start);
        time_t time2 = std::mktime(&end);
        double secondsDiff = difftime(time2, time1);
        int daysDiff = (static_cast<int>(secondsDiff / (60 * 60 * 24))) + 1;
        std::cout << "日期相差天数: " << daysDiff << " 天" << std::endl;
        return daysDiff;
    }
};


tm createTM(int year, int month, int day)
{
    tm ret;
    ret.tm_year = year - 1900;
    ret.tm_mon = month;
    ret.tm_mday = day;
    return ret;
}
TEST(First, Oneday)
{   
    
    tm start = createTM(2023, 4, 1);

    tm end = createTM(2023, 4, 1);
    BudgetRepo repo;
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 100.);
}


TEST(First, Twodays)
{
    tm start = createTM(2023, 4, 1);
    tm end = createTM(2023, 4, 2);
    BudgetRepo repo({ {"202304", 3000}, {"202305", 3100}});
    BudgetService serviece(repo);
    ASSERT_EQ(serviece.query(start, end), 200.);
}