#include "pipex.h"

char	*ft_strdup(const char *s1)
{
	size_t		i;
	char		*s;

	i = 0;
	s = (char *)malloc(ft_strlen(s1) + 1);
	if (!s)
		return (NULL);
	while (i < ft_strlen(s1))
	{
		s[i] = s1[i];
		i++;
	}
	s[i] = '\0';
	return (s);
}
char *ft_strcpy(char *dest,char *src)
{
    int i = 0;

    while (src[i])
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}
char *ft_strcat(char *dest,char *src)
{
    int i;
    int j;

    i = 0;
    j = 0;
    while(dest[i])
        i++;
    while(src[j])
    {
        dest[i + j] = src[j];
        j++;
    }
    dest[i + j] = '\0';
    return dest;
}



