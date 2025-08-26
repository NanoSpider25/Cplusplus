#include "1DDS.h"

// Constructor implementation
DS1D::DS1D()
	: size(1), offLoadFrom(0), recursiveSplitEvery(3),
	recursiveSplitHowManySplits(4), recursiveSplitMaxAmountOfSplits(20),
	nestedSplit(true), concurrentRecursion(false),
	openStructure(true), depth(1), splitCounter(0), copyPasteInfinitely(true)
{
	//ctor
}


std::any wrapLeaves(const std::any& item) {
	if (item.type() == typeid(std::vector<std::any>)) {
		const auto& innerVec = std::any_cast<const std::vector<std::any>&>(item);
		std::vector<std::any> wrappedVec;
		for (const auto& elem : innerVec) {
			wrappedVec.push_back(wrapLeaves(elem));  // recurse
		}
		return wrappedVec;
	}
	else {
		// Wrap any leaf value in a vector
		return std::vector<std::any>{ item };
	}
}

void DS1D::addLayer() {
	vec = std::any_cast<std::vector<std::any>>(wrapLeaves(vec));
}

void displayVecTree(const std::vector<std::any>& v, const std::string& prefix = "", bool isLast = true) {
	std::string branch = isLast ? "'-- " : "|____";
	std::cout << prefix << branch << "\n";

	for (size_t i = 0; i < v.size(); ++i) {
		bool last = (i == v.size() - 1);
		std::string newPrefix = prefix + (isLast ? "    " : "|   ");
		std::string elemBranch = last ? "'-- " : "|-- ";

		const auto& element = v[i];
		if (element.type() == typeid(int)) {
			std::cout << newPrefix << elemBranch << std::any_cast<int>(element) << "\n";
		}
		else if (element.type() == typeid(double)) {
			std::cout << newPrefix << elemBranch << std::any_cast<double>(element) << "\n";
		}
		else if (element.type() == typeid(std::string)) {
			std::cout << newPrefix << elemBranch << "\"" << std::any_cast<std::string>(element) << "\"\n";
		}
		else if (element.type() == typeid(char)) {
			std::cout << newPrefix << elemBranch << "'" << std::any_cast<char>(element) << "'\n";
		}
		else if (element.type() == typeid(std::vector<std::any>)) {
			displayVecTree(std::any_cast<std::vector<std::any>>(element), newPrefix, last);
		}
		else {
			std::cout << newPrefix << elemBranch << "<unknown type>\n";
		}
	}

	std::cout << prefix << (isLast ? "    " : "|   ") << "\n";
}

void displayVec(const std::vector<std::any>& v, int indent = 0) {
	std::string pad(indent, ' ');
	std::cout << pad << "[";
	for (const auto& element : v) {
		std::cout << pad << "  ";
		if (element.type() == typeid(int)) {
			std::cout << std::any_cast<int>(element);
		}
		else if (element.type() == typeid(double)) {
			std::cout << std::any_cast<double>(element);
		}
		else if (element.type() == typeid(std::string)) {
			std::cout << "\"" << std::any_cast<std::string>(element) << "\"";
		}
		else if (element.type() == typeid(char)) {
			std::cout << "'" << std::any_cast<char>(element) << "'";
		}
		else if (element.type() == typeid(std::vector<std::any>)) {
			displayVec(std::any_cast<std::vector<std::any>>(element), indent + 1);
		}
		else {
			std::cout << "<unknown type>";
		}
		std::cout << ", ";
	}
	std::cout << pad << "]\n";
}

int DS1D::getSizeAt(std::vector<int> indicies) {
	std::vector<std::any>* vecT = &vec;
	for (int i = 0; i < indicies.size(); i++) {
		std::vector<std::any>& innerVec = std::any_cast<std::vector<std::any>&>((*vecT)[indicies[i]]);
		vecT = &innerVec;
	}
	return vecT->size(); // Return the size of the inner vector at the specified indices
}

bool DS1D::findValidIndex(std::vector<int>& indicies, int depth, int maxSplit, std::vector<int>& current) {
	if (depth == indicies.size()) {
		if (DS1D::getSizeAt(current) < maxSplit) {
			indicies = current;
			return true;
		}
		return false;
	}

	for (int i = 0; i < maxSplit; i++) {
		current[depth] = i;
		if (findValidIndex(indicies, depth + 1, maxSplit, current)) {
			return true;
		}
	}

	return false;
}


void DS1D::recursive_index_sub(std::any value) {
	if ((recursiveSplitCounter == 0 && !nestedSplit) || (firstNest)) {
		addLayer();
		if (nestedSplit && firstNest && !copyPasteInfinitely) {
			indicies[0] = vec.size() - 1; // Set the index for the current split
			
		}
		firstNest = false; // Reset firstNest after adding the first layer
	}
	recursiveSplitCounter++; // Increment the recursive split counter
	std::vector<std::any>* vecT = &vec;
	for (int i = 0; i < indicies.size(); i++) {
		std::vector<std::any>& innerVec = std::any_cast<std::vector<std::any>&>((*vecT)[indicies[i]]);
		vecT = &innerVec;
	}
	if (nestedSplit && copyPasteInfinitely) {
		if ((vecT->size()) % recursiveSplitEvery != 0 && vecT->size() != 0) {
			vecT->push_back(value); // Add the value to the inner vector if not at split point
			display();
			return;
		}
		else if ((vecT->size()) == recursiveSplitEvery) {
			std::vector<std::any> tempVec; // Copy the current inner vector
			tempVec.push_back(value); // Add the value to the temporary vector
			vecT->push_back(tempVec); // Add the value to the inner vector if not at split point
			indicies.push_back(vecT->size() - 1); // Add the index of the new inner vector
			return;
		}
	}
	if (vecT->size() < recursiveSplitHowManySplits) {
		vecT->push_back(value); // Add the value to the inner vector
	}
	else {
		recursiveSplitCounter = 0;
		if (nestedSplit) {
			
			std::vector<int> temp = indicies; // starting point
			bool bPush = findValidIndex(indicies, 1, recursiveSplitHowManySplits, temp);

			if(!bPush) {
				addLayer(); // Add a new layer if the last index reached max splits
				indicies.push_back(0); // Add a new index if the first index reached max splits
				for(int i = 1; i < indicies.size() - 1; i++) {
					indicies[i] = 0; // Reset all previous indices to 0
				}
			}
							
			recursive_index_sub(value); // Call the recursive indexing function
			
		}
		else {
			vec.push_back(value); // Add the value to the inner vector if max splits reached
			splitCounter--; // Decrement the split counter
		}
	}

}

void DS1D::add(std::any value) {
	size++; // Increment the size of the structure
	if (recursiveSplitEvery == -1) {
		vec.push_back(value); // Add the value to the vector
	}
	else {
		if (vec.size() % recursiveSplitEvery == 0 && vec.size() != 0) {
			if (splitCounter < recursiveSplitMaxAmountOfSplits || recursiveSplitMaxAmountOfSplits < 0) {
				splitCounter++; // Increment the split counter
				if(!nestedSplit)
					indicies[0] = vec.size() - 1; // Set the index for the current split
				recursive_index_sub(value); // Call the recursive indexing function
			}
			else {
				vec.push_back(value); // Add the value to the vector if max splits reached
			}
		}
		else {
			vec.push_back(value); // Add the value to the vector
		}
	}
	//display(); // Display the initial state of the vector
}

void DS1D::display() {
	//displayVec(vec); // Display the contents of the vector
	//cout << "\n\n\n";
	displayVecTree(vec); // Display the contents of the vector as a tree structure
	//cout << "Indicies: "; 
	//for(int i = 0; i < indicies.size(); i++)
	//	cout << "| " << indicies[i];
	cout << "\nSize: " << size << " | Split Counter: " << splitCounter << endl;
	cout << "\n\n\n";
}


DS1D::~DS1D()
{
	//dtor
}

int main() {
	DS1D ds;
	ds.add(1); // Add an integer value
	ds.add(2.5); // Add a double value
	ds.add(std::string("Hello")); // Add a string value
	ds.add('A'); // Add a character value
	ds.add(11); // Add an integer value
	ds.add(2.51); // Add a double value
	ds.add(std::string("Hello1")); // Add a string value
	ds.add('B'); // Add a character value
	ds.add(std::string("Hello2")); // Add a string value
	ds.add('C'); // Add a character value
	ds.add('D'); // Add a character value
	ds.add(12); // Add an integer value
	ds.add(13); // Add an integer value
	ds.add(14); // Add an integer value
	ds.add(15); // Add an integer value
	ds.add(16); // Add an integer value
	ds.add(17); // Add an integer value
	ds.add(18); // Add an integer value
	ds.add(19); // Add an integer value
	ds.add(20); // Add an integer value
	ds.add(21); // Add an integer value
	ds.add(22); // Add an integer value
	ds.add(23); // Add an integer value
	ds.add(24); // Add an integer value
	
	ds.display(); // Display the state of the vector after adding values
	return 0;
}