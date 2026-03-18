#pragma once
#include <string>
#include <vector>
#include <stdexcept>


class Review {
private:
    int m_orderId;
    int m_reviewerId;
    std::string m_comment;
    int m_rating;

    static void ValidateRating(int rating);
public:
    // Constructor
    Review(); 
    Review(int orderId, int reviewerId, const std::string& comment, int rating);

    int GetOrderId() const { 
        return m_orderId; 
    }
    int GetReviewerId() const { 
        return m_reviewerId; 
    }
    std::string GetComment() const { 
        return m_comment; 
    }
    int GetRating() const { 
        return m_rating; 
    }

    // Setters
    void SetOrderId(const int orderId);
    void SetReviewerId(const int reviewerId);
    void SetComment(const std::string& comment);
    void SetRating(const int rating);

    // Helper for database (.tsv file based)
    std::string Serialize() const;
    static Review Deserialize(const std::string& line);
};

// Exceptions
class ReviewException : public std::runtime_error {
public:
    explicit ReviewException(const std::string& msg) : std::runtime_error(msg) {}
};