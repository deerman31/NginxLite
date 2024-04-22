#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

int	main() {
	char	*input;

	while (true) {
		input = readline("入力してね: ");
		std::cout<< input << std::endl;
		add_history(input);
		free(input);
	}
	return 0;
}
