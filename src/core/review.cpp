#include "review.h"
#include<sstream>
#include<string>
#include<algorithm>

void Review::ValidateRating(int rating) {
    if (rating < 1 || rating > 5) {
        throw ReviewException("Rating must be between 1 and 5.");
    }
}

Review::Review() : m_orderId(-1), m_reviewerId(-1), m_comment(""), m_rating(-1) {}

Review::Review(int orderId, int reviewerId, const std::string& comment, int rating)
    : m_orderId(orderId), m_reviewerId(reviewerId), m_comment(comment), m_rating(rating) {
    ValidateRating(rating);
}

void Review::SetOrderId(const int orderId) { 
    m_orderId = orderId; 
}
void Review::SetReviewerId(const int reviewerId) { 
    m_reviewerId = reviewerId; 
}
void Review::SetComment(const std::string& comment) { 
    m_comment = comment; 
}
void Review::SetRating(const int rating) { 
    ValidateRating(rating);
    m_rating = rating; 
}

// Review Parsers for Database
std::string Review::Serialize() const {

    
    std::string safeComment;
    for (char c : m_comment) {
        if (c == '\n') {
            safeComment += "\\n";
        }     
        else if (c == '\r') {
            safeComment += "\\r";
        }
        else {
            safeComment += c;
        }   
    }
    std::ostringstream oss;
    oss << m_orderId << "\t" << m_reviewerId << "\t" << m_rating << "\t" << safeComment;
    return oss.str();
}

Review Review::Deserialize(const std::string &line) {
    std::istringstream iss(line);
    std::string token;
    int orderId, reviewerId, rating;

    std::getline(iss, token, '\t');
    orderId = std::stoi(token);
    std::getline(iss, token, '\t');
    reviewerId = std::stoi(token);
    std::getline(iss, token, '\t');
    rating = std::stoi(token);

    std::string raw;
    std::getline(iss, raw);


    std::string comment;
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            if (raw[i+1] == 'n')  { comment += '\n'; ++i; }
            else if (raw[i+1] == 'r') { comment += '\r'; ++i; }
            else comment += raw[i];
        } 
        else {
            comment += raw[i];
        }
    }

    return Review(orderId, reviewerId, comment, rating);
}