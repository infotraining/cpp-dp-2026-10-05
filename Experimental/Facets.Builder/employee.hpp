#ifndef EMPLOYEE_HPP
#define EMPLOYEE_HPP

#include <optional>
#include <string>
#include <utility>

namespace NoBuilder
{
    class Employee
    {
        std::string first_name_, last_name_;
        std::optional<std::string> email_, phone_;
        std::optional<std::string> address_, postal_code_, city_;
        std::optional<std::string> department_, position_, manager_;
        std::optional<double> salary_;

    public:
        Employee(std::string first_name, std::string last_name)
            : first_name_{std::move(first_name)}
            , last_name_{std::move(last_name)}
        {
        }

        Employee(std::string first_name, std::string last_name, std::string email)
            : Employee{std::move(first_name), std::move(last_name)}
        {
            email_ = std::move(email);
        }

        Employee(std::string first_name, std::string last_name, std::string email, std::string phone)
            : Employee{std::move(first_name), std::move(last_name), std::move(email)}
        {
            phone_ = std::move(phone);
        }

        Employee(std::string first_name, std::string last_name, std::string address, std::string postal_code, std::string city)
            : Employee{std::move(first_name), std::move(last_name)}
        {
            address_ = std::move(address);
            postal_code_ = std::move(postal_code);
            city_ = std::move(city);
        }

        Employee(std::string first_name, std::string last_name, std::string department, std::string position, double salary)
            : Employee{std::move(first_name), std::move(last_name)}
        {
            department_ = std::move(department);
            position_ = std::move(position);
            salary_ = salary;
        }

        Employee(std::string first_name, std::string last_name, std::string email, std::string phone,
            std::string address, std::string postal_code, std::string city)
            : Employee{std::move(first_name), std::move(last_name), std::move(address), std::move(postal_code), std::move(city)}
        {
            email_ = std::move(email);
            phone_ = std::move(phone);
        }

        Employee(std::string first_name, std::string last_name, std::string email, std::string phone,
            std::string address, std::string postal_code, std::string city,
            std::string department, std::string position, std::string manager, double salary)
            : Employee{std::move(first_name), std::move(last_name), std::move(email), std::move(phone),
                  std::move(address), std::move(postal_code), std::move(city)}
        {
            department_ = std::move(department);
            position_ = std::move(position);
            manager_ = std::move(manager);
            salary_ = salary;
        }

        std::string description() const
        {
            std::string result = first_name_ + " " + last_name_;
            result += " <" + email_.value_or("-") + "> tel. " + phone_.value_or("-");
            result += " - " + address_.value_or("-") + ", " + postal_code_.value_or("-") + " " + city_.value_or("-");
            result += " - " + department_.value_or("-") + "/" + position_.value_or("-");
            result += " (manager: " + manager_.value_or("-") + ")";
            result += " salary: " + (salary_ ? std::to_string(*salary_) : std::string{"-"});
            return result;
        }
    };
}

namespace WithBuilder
{
    class EmployeeBuilder;
    class EmployeeContactBuilder;
    class EmployeeAddressBuilder;
    class EmployeeJobBuilder;

    class Employee
    {
        std::string first_name_, last_name_;
        std::optional<std::string> email_, phone_;
        std::optional<std::string> address_, postal_code_, city_;
        std::optional<std::string> department_, position_, manager_;
        std::optional<double> salary_;

        Employee() = default;

    public:
        std::string description() const
        {
            std::string result = first_name_ + " " + last_name_;
            result += " <" + email_.value_or("-") + "> tel. " + phone_.value_or("-");
            result += " - " + address_.value_or("-") + ", " + postal_code_.value_or("-") + " " + city_.value_or("-");
            result += " - " + department_.value_or("-") + "/" + position_.value_or("-");
            result += " (manager: " + manager_.value_or("-") + ")";
            result += " salary: " + (salary_ ? std::to_string(*salary_) : std::string{"-"});
            return result;
        }

        friend class EmployeeBuilder;
        friend class EmployeeContactBuilder;
        friend class EmployeeAddressBuilder;
        friend class EmployeeJobBuilder;

        static EmployeeBuilder create(std::string first_name, std::string last_name);
    };

    class EmployeeBuilderBase
    {
    protected:
        Employee& employee_;

        explicit EmployeeBuilderBase(Employee& e)
            : employee_{e}
        {
        }

    public:
        operator Employee&&()
        {
            return std::move(employee_);
        }

        EmployeeContactBuilder contact();
        EmployeeAddressBuilder lives();
        EmployeeJobBuilder works();
    };

    class EmployeeContactBuilder : public EmployeeBuilderBase
    {
        using self = EmployeeContactBuilder;

    public:
        explicit EmployeeContactBuilder(Employee& e)
            : EmployeeBuilderBase{e}
        {
        }

        self&& email(std::string email)
        {
            employee_.email_ = std::move(email);
            return std::move(*this);
        }

        self&& phone(std::string phone)
        {
            employee_.phone_ = std::move(phone);
            return std::move(*this);
        }
    };

    class EmployeeAddressBuilder : public EmployeeBuilderBase
    {
        using self = EmployeeAddressBuilder;

    public:
        explicit EmployeeAddressBuilder(Employee& e)
            : EmployeeBuilderBase{e}
        {
        }

        self&& at(std::string street_address)
        {
            employee_.address_ = std::move(street_address);
            return std::move(*this);
        }

        self&& in(std::string city)
        {
            employee_.city_ = std::move(city);
            return std::move(*this);
        }

        self&& with_postal_code(std::string postal_code)
        {
            employee_.postal_code_ = std::move(postal_code);
            return std::move(*this);
        }
    };

    class EmployeeJobBuilder : public EmployeeBuilderBase
    {
        using self = EmployeeJobBuilder;

    public:
        explicit EmployeeJobBuilder(Employee& e)
            : EmployeeBuilderBase{e}
        {
        }

        self&& in_department(std::string department)
        {
            employee_.department_ = std::move(department);
            return std::move(*this);
        }

        self&& as(std::string position)
        {
            employee_.position_ = std::move(position);
            return std::move(*this);
        }

        self&& reporting_to(std::string manager)
        {
            employee_.manager_ = std::move(manager);
            return std::move(*this);
        }

        self&& earning(double salary)
        {
            employee_.salary_ = salary;
            return std::move(*this);
        }
    };

    class EmployeeBuilder
    {
        Employee e_;

    public:
        EmployeeBuilder(std::string first_name, std::string last_name)
        {
            e_.first_name_ = std::move(first_name);
            e_.last_name_ = std::move(last_name);
        }

        EmployeeContactBuilder contact() { return EmployeeContactBuilder{e_}; }
        EmployeeAddressBuilder lives() { return EmployeeAddressBuilder{e_}; }
        EmployeeJobBuilder works() { return EmployeeJobBuilder{e_}; }

        operator Employee&&() { return std::move(e_); }
    };

    inline EmployeeContactBuilder EmployeeBuilderBase::contact() { return EmployeeContactBuilder{employee_}; }
    inline EmployeeAddressBuilder EmployeeBuilderBase::lives() { return EmployeeAddressBuilder{employee_}; }
    inline EmployeeJobBuilder EmployeeBuilderBase::works() { return EmployeeJobBuilder{employee_}; }

    inline EmployeeBuilder Employee::create(std::string first_name, std::string last_name)
    {
        return EmployeeBuilder{std::move(first_name), std::move(last_name)};
    }
}

#endif // EMPLOYEE_HPP
