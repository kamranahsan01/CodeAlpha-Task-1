#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <conio.h>
#include <cstdio>
#include <iomanip>
#include <sstream>
using namespace std;
class LoginSystem {
public:
	enum class FormResult { Continue, Home, Exit };
private:
	struct UserRecord {
		string username;
		string passwordHash;
		string firstName;
		string lastName;
		string email;
		bool hasProfile = false;
	};
	const string userFile = "users.txt";
	const string temporaryUserFile = "users_temp.txt";
	FormResult readInput(const string& prompt, string& value) const {
		cout << prompt << " (type /home for home or /exit to quit): ";
		if (!getline(cin, value)) {
			return FormResult::Exit;
		}
		if (value == "/home") {
			return FormResult::Home;
		}
		if (value == "/exit") {
			return FormResult::Exit;
		}
		return FormResult::Continue;
	}
	FormResult readPassword(const string& prompt, string& password) const {
		cout << prompt << " (Esc: home, F10: exit): ";
		int character;
		while ((character = _getch()) != '\r') {
			if (character == 27) {
				cout << '\n';
				return FormResult::Home;
			}
			if (character == 0 || character == 224) {
				if (_getch() == 68) {
					cout << '\n';
					return FormResult::Exit;
				}
				continue;
			}
			if (character == '\b') {
				if (!password.empty()) {
					password.pop_back();
					cout << "\b \b";
				}
			} else if (character >= 32 && character <= 126) {
				password += static_cast<char>(character);
				cout << '*';
			}
		}
		cout << '\n';
		return FormResult::Continue;
	}
	string hashPassword(const string& password) const {
		unsigned long long hash = 1469598103934665603ULL;
		for (unsigned char character : password) {
			hash ^= character;
			hash *= 1099511628211ULL;
		}
		return to_string(hash);
	}
	bool isValidUsername(const string& username) const {
		if (username.empty()) {
			return false;
		}
		for (char character : username) {
			if (!isalnum(static_cast<unsigned char>(character)) && character != '_') {
				return false;
			}
		}
		return true;
	}
	bool isValidEmail(const string& email) const {
		if (email.empty() || email.size() > 254) {
			return false;
		}
		const size_t atPosition = email.find('@');
		if (atPosition == string::npos || atPosition == 0 || atPosition != email.rfind('@') || atPosition > 64) {
			return false;
		}
		const string localPart = email.substr(0, atPosition);
		const string domain = email.substr(atPosition + 1);
		if (localPart.front() == '.' || localPart.back() == '.' || localPart.find("..") != string::npos) {
			return false;
		}
		for (char character : localPart) {
			const unsigned char value = static_cast<unsigned char>(character);
			if (!isalnum(value) && string(".!#$%&'*+-/=?^_`{|}~").find(character) == string::npos) {
				return false;
			}
		}
		if (domain.find('.') == string::npos || domain.front() == '.' || domain.back() == '.' || domain.find("..") != string::npos) {
			return false;
		}
		size_t labelStart = 0;
		while (labelStart < domain.size()) {
			const size_t dotPosition = domain.find('.', labelStart);
			const size_t labelEnd = dotPosition == string::npos ? domain.size() : dotPosition;
			const string label = domain.substr(labelStart, labelEnd - labelStart);
			if (label.empty() || label.size() > 63 || label.front() == '-' || label.back() == '-') {
				return false;
			}
			for (char character : label) {
				if (!isalnum(static_cast<unsigned char>(character)) && character != '-') {
					return false;
				}
			}
			if (dotPosition == string::npos) {
				break;
			}
			labelStart = dotPosition + 1;
		}
		return true;
	}
	bool isValidPassword(const string& password) const {
		return password.length() >= 6;
	}
	bool parseUserRecord(const string& line, UserRecord& record) const {
		istringstream input(line);
		if (!(input >> record.username >> record.passwordHash)) {
			return false;
		}
		record.hasProfile = static_cast<bool>(input >> quoted(record.firstName) >> quoted(record.lastName) >> quoted(record.email));
		return true;
	}
	bool userExists(const string& username) const {
		ifstream file(userFile);
		string line;
		UserRecord record;
		while (getline(file, line)) {
			if (parseUserRecord(line, record) && record.username == username) {
				return true;
			}
		}
		return false;
	}
	string normalizeEmail(const string& email) const {
		string normalized = email;
		for (char& character : normalized) {
			character = static_cast<char>(tolower(static_cast<unsigned char>(character)));
		}
		return normalized;
	}
	bool emailExists(const string& email) const {
		ifstream file(userFile);
		string line;
		UserRecord record;
		while (getline(file, line)) {
			if (parseUserRecord(line, record) && record.hasProfile && normalizeEmail(record.email) == normalizeEmail(email)) {
				return true;
			}
		}
		return false;
	}
	void writeUserRecord(ostream& output, const UserRecord& record) const {
		output << record.username << ' ' << record.passwordHash;
		if (record.hasProfile) {
			output << ' ' << quoted(record.firstName) << ' ' << quoted(record.lastName) << ' ' << quoted(record.email);
		}
		output << '\n';
	}
public:
	FormResult registerUser() const {
		string firstName;
		string lastName;
		string email;
		string username;
		string password;
		string confirmPassword;
		FormResult result;

		cout << "\n===== Registration =====\n";
		result = readInput("Enter your first name", firstName);
		if (result != FormResult::Continue) return result;
		if (firstName.empty()) {
			cout << "First name cannot be empty.\n";
			return FormResult::Home;
		}
		result = readInput("Enter your last name", lastName);
		if (result != FormResult::Continue) return result;
		if (lastName.empty()) {
			cout << "Last name cannot be empty.\n";
			return FormResult::Home;
		}

		while (true) {
			result = readInput("Enter your email address", email);
			if (result != FormResult::Continue) return result;
			if (!isValidEmail(email)) {
				cout << "Invalid email format. Please enter a valid international email address.\n";
				continue;
			}
			if (emailExists(email)) {
				cout << "Email already taken. Please use another email address.\n";
				continue;
			}
			break;
		}

		while (true) {
			result = readInput("Choose a username (letters, numbers, and underscores only)", username);
			if (result != FormResult::Continue) return result;
			if (!isValidUsername(username)) {
				cout << "Username must contain only letters, numbers, or underscores.\n";
				continue;
			}
			if (userExists(username)) {
				cout << "Username already taken. Please choose another username.\n";
				continue;
			}
			break;
		}

		while (true) {
			password.clear();
			confirmPassword.clear();
			result = readPassword("Enter a password (minimum 6 characters)", password);
			if (result != FormResult::Continue) return result;
			if (!isValidPassword(password)) {
				cout << "Password must contain at least 6 characters.\n";
				continue;
			}
			result = readPassword("Confirm your password", confirmPassword);
			if (result != FormResult::Continue) return result;
			if (password != confirmPassword) {
				cout << "Passwords do not match. Please try again.\n";
				continue;
			}
			break;
		}

		ofstream file(userFile, ios::app);
		if (!file) {
			cout << "Registration failed: unable to open the user database.\n";
			return FormResult::Continue;
		}
		UserRecord record{ username, hashPassword(password), firstName, lastName, email, true };
		writeUserRecord(file, record);
		if (!file) {
			cout << "Registration failed: unable to save the user record.\n";
			return FormResult::Continue;
		}
		cout << "Registration successful.\n";
		return FormResult::Continue;
	}
	FormResult loginUser() const {
		string username;
		string password;
		cout << "\n===== Login =====\n";
		FormResult result = readInput("Enter your username", username);
		if (result != FormResult::Continue) return result;
		if (!userExists(username)) {
			cout << "Login failed: wrong username.\n";
			return FormResult::Continue;
		}
		result = readPassword("Enter your password", password);
		if (result != FormResult::Continue) return result;
		ifstream file(userFile);
		string line;
		UserRecord record;
		const string enteredPasswordHash = hashPassword(password);
		while (getline(file, line)) {
			if (parseUserRecord(line, record) && record.username == username && record.passwordHash == enteredPasswordHash) {
				cout << "Login successful. Welcome, " << username << "!\n";
				return FormResult::Continue;
			}
		}
		cout << "Login failed: incorrect password.\n";
		return FormResult::Continue;
	}
	FormResult changePassword() const {
		string username;
		string currentPassword;
		string newPassword;
		string confirmPassword;
		cout << "\n===== Change Password =====\n";
		FormResult result = readInput("Enter your username", username);
		if (result != FormResult::Continue) return result;
		if (!userExists(username)) {
			cout << "Password change failed: wrong username.\n";
			return FormResult::Continue;
		}
		result = readPassword("Enter your current password", currentPassword);
		if (result != FormResult::Continue) return result;
		bool passwordMatches = false;
		ifstream inputFile(userFile);
		string line;
		UserRecord record;
		const string currentPasswordHash = hashPassword(currentPassword);
		while (getline(inputFile, line)) {
			if (parseUserRecord(line, record) && record.username == username && record.passwordHash == currentPasswordHash) {
				passwordMatches = true;
				break;
			}
		}
		inputFile.close();
		if (!passwordMatches) {
			cout << "Password change failed: incorrect current password.\n";
			return FormResult::Continue;
		}

		while (true) {
			newPassword.clear();
			confirmPassword.clear();
			result = readPassword("Enter your new password (minimum 6 characters)", newPassword);
			if (result != FormResult::Continue) return result;
			if (!isValidPassword(newPassword)) {
				cout << "Password must contain at least 6 characters.\n";
				continue;
			}
			result = readPassword("Confirm your new password", confirmPassword);
			if (result != FormResult::Continue) return result;
			if (newPassword != confirmPassword) {
				cout << "Passwords do not match. Please try again.\n";
				continue;
			}
			break;
		}

		ifstream source(userFile);
		ofstream destination(temporaryUserFile);
		if (!source || !destination) {
			cout << "Password change failed: unable to update the user database.\n";
			return FormResult::Continue;
		}
		while (getline(source, line)) {
			if (parseUserRecord(line, record)) {
				if (record.username == username) {
					record.passwordHash = hashPassword(newPassword);
				}
				writeUserRecord(destination, record);
			} else {
				destination << line << '\n';
			}
		}
		source.close();
		destination.close();
		if (!destination || remove(userFile.c_str()) != 0 || rename(temporaryUserFile.c_str(), userFile.c_str()) != 0) {
			remove(temporaryUserFile.c_str());
			cout << "Password change failed: unable to update the user database.\n";
			return FormResult::Continue;
		}
		cout << "Password changed successfully.\n";
		return FormResult::Continue;
	}
};
int main() {
	LoginSystem loginSystem;
	bool shouldExit = false;
	while (!shouldExit) {
		cout << "\n===== Login and Registration System =====\n";
		cout << "1. Register\n";
		cout << "2. Login\n";
		cout << "3. Change Password\n";
		cout << "4. Exit\n";
		cout << "Choose an option: ";
		string choice;
		if (!getline(cin, choice)) {
			break;
		}
		LoginSystem::FormResult result = LoginSystem::FormResult::Continue;
		if (choice == "1") {
			result = loginSystem.registerUser();
		} else if (choice == "2") {
			result = loginSystem.loginUser();
		} else if (choice == "3") {
			result = loginSystem.changePassword();
		} else if (choice == "4") {
			shouldExit = true;
		} else {
			cout << "Invalid option. Please choose 1, 2, 3, or 4.\n";
		}
		if (result == LoginSystem::FormResult::Exit) {
			shouldExit = true;
		}
	}
	cout << "Goodbye!\n";
	return 0;
}
