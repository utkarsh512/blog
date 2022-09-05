# Changing JPG/PDF images to EPS

## PDF to EPS

Install `ghostscript`:

```bash
sudo apt install ghostscript
```

To convert `input.pdf` to `output.eps`, run following command:

```bash
gs -q -dNOCACHE -dNOPAUSE -dBATCH -dSAFER -sDEVICE=eps2write -sOutputFile=output.eps input.pdf
```

## JPG to EPS

First convert JPG to PDF, then convert obtained PDF file to EPS using commands given above.

Install `imagemagick` as:

```bash
sudo apt-get install imagemagick
```

For the first-time use only, change the permission by opening `policy.xml` as:

```bash
sudo nano /etc/ImageMagick-6/policy.xml
```

and comment the line 

```xml
<policy domain="coder" rights="none" pattern="PDF" />
```

To convert `input.jpg` to `output.pdf`, run follwoing command:

```bash
convert input.jpg output.pdf
```
