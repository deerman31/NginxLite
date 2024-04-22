use strict;
use warnings;
use feature 'say';

my $f = "Fizz";
my $b = "Buzz";
say "Content-Type: text/plain";
say "";
for my $i (1..100) {
	if ($i % 3 == 0 && $i % 5 == 0) {
		say $f, $b;
	}
	elsif ($i % 3 == 0) {
		say $f;
	}
	elsif ($i % 5 == 0) {
		say $b;
	}
	else {
		say $i;
	}
}
