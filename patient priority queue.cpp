#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

namespace UI {
	const string Reset = "\033[0m";
	const string Cyan = "\033[36m";
	const string Green = "\033[32m";
	const string Yellow = "\033[33m";
	const string Red = "\033[31m";
	const string Purple = "\033[35m";
	const string Bold = "\033[1m";
	const string Dim = "\033[2m";

	void ClearScreen() {
		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif
	}

	void Pause() {
		cout << Dim << "\nPress Enter to return to dashboard..." << Reset;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.get();
	}

	void Header(const string& subtitle = "Emergency priority management") {
		cout << Cyan << Bold;
		cout << "\n+==============================================================+\n";
		cout << "|                  HOSPITAL PRIORITY QUEUE                    |\n";
		cout << "+==============================================================+\n" << Reset;
		cout << Purple << "  " << subtitle << Reset << "\n\n";
	}

	void Section(const string& title) {
		cout << Cyan << Bold << "\n-- " << title << " " << string(58 - title.length(), '-') << Reset << "\n";
	}

	void Success(const string& message) {
		cout << Green << Bold << "\n[SUCCESS] " << Reset << message << "\n";
	}

	void Warning(const string& message) {
		cout << Yellow << Bold << "\n[NOTICE] " << Reset << message << "\n";
	}

	void Error(const string& message) {
		cout << Red << Bold << "\n[ERROR] " << Reset << message << "\n";
	}

	string SeverityColor(int severity) {
		if (severity >= 8) return Red;
		if (severity >= 5) return Yellow;
		return Green;
	}

	string SeverityLabel(int severity) {
		if (severity >= 8) return "Critical";
		if (severity >= 5) return "Urgent";
		return "Stable";
	}

	void SeverityBar(int severity) {
		cout << SeverityColor(severity);
		for (int i = 1; i <= 10; i++) {
			cout << (i <= severity ? '#' : '.');
		}
		cout << Reset;
	}

	int ReadInt(const string& prompt, int minValue, int maxValue) {
		int value;

		while (true) {
			cout << Bold << prompt << Reset;
			if (cin >> value && value >= minValue && value <= maxValue) {
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				return value;
			}

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			Error("Enter a number from " + to_string(minValue) + " to " + to_string(maxValue) + ".");
		}
	}

	string ReadLine(const string& prompt) {
		string value;

		while (true) {
			cout << Bold << prompt << Reset;
			getline(cin, value);

			if (!value.empty()) {
				return value;
			}

			Error("This field cannot be empty.");
		}
	}
}

class Patient {
public:
	string Name;
	int Age;
	int Severity;
	Patient* Next;

	Patient() {
		Next = NULL;
	}

	void UpdateDetails(string name, int age, int severity) {
		Name = name;
		Age = age;
		Severity = severity;
	}
};

class QueuePiroity {
public:
	Patient* Head;
	Patient* Tail;

	QueuePiroity() {
		Head = NULL;
		Tail = NULL;
	}

	bool isEmpty() {
		return Head == NULL;
	}

	int Count() {
		int total = 0;
		Patient* temp = Head;

		while (temp != NULL) {
			total++;
			temp = temp->Next;
		}

		return total;
	}

	void Enqueue(string name, int age, int severity) {
		Patient* newPatient = new Patient();
		newPatient->Name = name;
		newPatient->Age = age;
		newPatient->Severity = severity;

		if (Head == NULL) {
			Head = newPatient;
			Tail = newPatient;
		}
		else if (severity > Head->Severity) {
			newPatient->Next = Head;
			Head = newPatient;
		}
		else {
			Patient* temp = Head;
			while (temp->Next != NULL && temp->Next->Severity >= severity) {
				temp = temp->Next;
			}

			newPatient->Next = temp->Next;
			temp->Next = newPatient;

			if (newPatient->Next == NULL) {
				Tail = newPatient;
			}
		}
	}

	bool Dequeue() {
		if (isEmpty()) {
			return false;
		}

		Patient* temp = Head;
		Head = Head->Next;
		delete temp;

		if (Head == NULL) {
			Tail = NULL;
		}

		return true;
	}

	void Display() {
		if (isEmpty()) {
			UI::Warning("No patients are waiting right now.");
			return;
		}

		cout << left;
		cout << "+----+----------------------+-----+------------+------------+\n";
		cout << "| #  | Patient              | Age | Status     | Severity   |\n";
		cout << "+----+----------------------+-----+------------+------------+\n";

		Patient* temp = Head;
		int position = 1;

		while (temp != NULL) {
			cout << "| " << setw(2) << position << " | "
				 << setw(20) << temp->Name.substr(0, 20) << " | "
				 << setw(3) << temp->Age << " | "
				 << setw(10) << UI::SeverityLabel(temp->Severity) << " | ";
			UI::SeverityBar(temp->Severity);
			cout << " |\n";

			temp = temp->Next;
			position++;
		}

		cout << "+----+----------------------+-----+------------+------------+\n";
	}
};

class Doctor {
public:
	string D_Name;
	int D_ID;
	string Specialization;

	void SetDoctorDetails(string name, int id, string specialization) {
		D_Name = name;
		D_ID = id;
		Specialization = specialization;
	}

	void GetDoctorDetails() {
		cout << "Doctor Name: " << D_Name << ", ID: " << D_ID << ", Specialization: " << Specialization << "\n";
	}

	void UpdateSpecialization(string specialization) {
		Specialization = specialization;
	}
};

class HospitalManager {
public:
	Patient P;
	QueuePiroity QP;
	vector<Doctor> Doctors;
	Doctor NewDoctor;

	void ADDPatient() {
		string name = UI::ReadLine("Patient name: ");
		int age = UI::ReadInt("Patient age: ", 1, 120);
		int severity = UI::ReadInt("Severity 1-10: ", 1, 10);

		QP.Enqueue(name, age, severity);
		UI::Success(name + " added to the priority queue.");
	}

	void RemovePatient() {
		if (QP.Dequeue()) {
			UI::Success("Highest priority patient moved to treatment.");
		}
		else {
			UI::Warning("Queue has no patients to remove.");
		}
	}

	void DisplayPatients() {
		QP.Display();
	}

	void AddDoctor() {
		string name = UI::ReadLine("Doctor name: ");
		int id = UI::ReadInt("Doctor ID: ", 1, 999999);
		string specialization = UI::ReadLine("Specialization: ");

		NewDoctor.SetDoctorDetails(name, id, specialization);
		Doctors.push_back(NewDoctor);
		UI::Success("Doctor added successfully.");
	}

	void DisplayDoctors() {
		if (Doctors.empty()) {
			UI::Warning("No doctors have been added yet.");
			return;
		}

		cout << left;
		cout << "+----+----------------------+----------+----------------------+\n";
		cout << "| #  | Doctor               | ID       | Specialization       |\n";
		cout << "+----+----------------------+----------+----------------------+\n";

		for (size_t i = 0; i < Doctors.size(); i++) {
			cout << "| " << setw(2) << i + 1 << " | "
				 << setw(20) << Doctors[i].D_Name.substr(0, 20) << " | "
				 << setw(8) << Doctors[i].D_ID << " | "
				 << setw(20) << Doctors[i].Specialization.substr(0, 20) << " |\n";
		}

		cout << "+----+----------------------+----------+----------------------+\n";
	}

	void SearchDoctorByID(int id) {
		for (const auto& doc : Doctors) {
			if (doc.D_ID == id) {
				UI::Success("Doctor found.");
				cout << "Name: " << doc.D_Name << "\nSpecialization: " << doc.Specialization << "\n";
				return;
			}
		}

		UI::Warning("Doctor with ID " + to_string(id) + " was not found.");
	}

	void editDoctorSpecialization(int id, string newSpecialization) {
		for (auto& doc : Doctors) {
			if (doc.D_ID == id) {
				doc.UpdateSpecialization(newSpecialization);
				UI::Success("Doctor specialization updated.");
				return;
			}
		}

		UI::Warning("Doctor with ID " + to_string(id) + " was not found.");
	}
};

class Menu {
public:
	HospitalManager HM;

	void DisplayStats() {
		cout << "+----------------------+----------------------+\n";
		cout << "| Waiting Patients     | " << setw(20) << HM.QP.Count() << " |\n";
		cout << "| Doctors Registered   | " << setw(20) << HM.Doctors.size() << " |\n";
		cout << "+----------------------+----------------------+\n\n";
	}

	void DisplayMenu() {
		int choice;

		do {
			UI::ClearScreen();
			UI::Header("Live triage dashboard - highest severity is served first");
			DisplayStats();

			cout << UI::Bold << "Choose an action\n" << UI::Reset;
			cout << "  " << UI::Green << "1" << UI::Reset << ". Add patient\n";
			cout << "  " << UI::Red << "2" << UI::Reset << ". Move next patient to treatment\n";
			cout << "  " << UI::Yellow << "3" << UI::Reset << ". View patient queue\n";
			cout << "  " << UI::Green << "4" << UI::Reset << ". Add doctor\n";
			cout << "  " << UI::Yellow << "5" << UI::Reset << ". View doctors\n";
			cout << "  " << UI::Cyan << "6" << UI::Reset << ". Search doctor by ID\n";
			cout << "  " << UI::Purple << "7" << UI::Reset << ". Edit doctor specialization\n";
			cout << "  8. Exit\n\n";

			choice = UI::ReadInt("Enter your choice: ", 1, 8);
			UI::ClearScreen();

			switch (choice) {
			case 1:
				UI::Header("Register a new patient");
				HM.ADDPatient();
				UI::Pause();
				break;
			case 2:
				UI::Header("Treatment desk");
				HM.RemovePatient();
				UI::Pause();
				break;
			case 3:
				UI::Header("Patient priority queue");
				HM.DisplayPatients();
				UI::Pause();
				break;
			case 4:
				UI::Header("Register a doctor");
				HM.AddDoctor();
				UI::Pause();
				break;
			case 5:
				UI::Header("Doctor directory");
				HM.DisplayDoctors();
				UI::Pause();
				break;
			case 6: {
				UI::Header("Find doctor");
				int id = UI::ReadInt("Doctor ID to search: ", 1, 999999);
				HM.SearchDoctorByID(id);
				UI::Pause();
				break;
			}
			case 7: {
				UI::Header("Update doctor specialization");
				int id = UI::ReadInt("Doctor ID to edit: ", 1, 999999);
				string newSpecialization = UI::ReadLine("New specialization: ");
				HM.editDoctorSpecialization(id, newSpecialization);
				UI::Pause();
				break;
			}
			case 8:
				UI::Header("Session closed");
				cout << UI::Green << "Thanks for using Hospital Priority Queue.\n" << UI::Reset;
				break;
			}
		} while (choice != 8);
	}
};

int main() {
	Menu menu;
	menu.DisplayMenu();
	return 0;
}
