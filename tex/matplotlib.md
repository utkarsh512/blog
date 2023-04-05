# How `matplotlib` should be used for $\LaTeX$
We often use `matplotlib` and `seaborn` to generate plots for the research papers. However, most of the conferences and journals have a specified rules for the `PDF` as well as the $\LaTeX$ source.

Follow these guidelines while generating plots to have a hassle-free paper submission:

```python
# Import plotting libraries
import matplotlib
from matplotlib import pyplot as plt
import seaborn as sns

# Change the default settings for matplotlib and seaborn
sns.set(style='darkgrid')                         # for increasing aesthetics of the plots
matplotlib.rcParams['figure.dpi'] = 300           # for increasing quality of the plots
matplotlib.rcParams['font.size'] = 18             # for uniform font size of the text
matplotlib.rcParams['figure.figsize'] = (18, 5)   # for uniform figure size throughout the paper
matplotlib.rcParams['pdf.fonttype'] = 42          # Remove Type3 fonts (they are often forbidden by most journals), uses TrueType fonts
matplotlib.rcParams['ps.fonttype'] = 42           # Remove Type3 fonts (they are often forbidden by most journals), uses TrueType fonts

# Always use this apporach while creating the plots
fig, ax = plt.subplots()

sns.lineplot(data=data, x='x', y='y', hue='hue', ax=ax)
ax.set_title(...)
ax.set_xlabel(...)
ax.set_ylabel(...)

# Always save the plots in pdf format (as some journals only use PDFLaTeX for compiling their source)
fig.savefig('plot.pdf', bbox_inches='tight')     
```
