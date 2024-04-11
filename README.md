<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->

<a name="readme-top"></a>

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Kevin-Heleodoro/Augmented-Museum">
    <img src="img/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Augmented Museum</h3>

  <p align="center">
    Augmented Reality application to display paintings on a surface.
    <br />
    <a href="https://github.com/Kevin-Heleodoro/Augmented-Museum"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/Kevin-Heleodoro/Augmented-Museum">View Demo</a>
    ·
    <a href="https://github.com/Kevin-Heleodoro/Augmented-Museum/issues">Report Bug</a>
    ·
    <a href="https://github.com/Kevin-Heleodoro/Augmented-Museum/issues">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

The idea of this project is to give a user the ability to create a virtual museum in whatever physical space they have (home, office, etc.). The goal is to create framed paintings/photos to be "hung" on a wall as one would normally do.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Built With

<!-- -   [![Next][Next.js]][Next-url]
-   [![React][React.js]][React-url]
-   [![Vue][Vue.js]][Vue-url]
-   [![Angular][Angular.io]][Angular-url]
-   [![Svelte][Svelte.dev]][Svelte-url]
-   [![Laravel][Laravel.com]][Laravel-url]
-   [![Bootstrap][Bootstrap.com]][Bootstrap-url]
-   [![JQuery][JQuery.com]][JQuery-url] -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->

## Getting Started

To get a local copy up and running follow these simple example steps. This application was built and tested on a Mac OS using g++ and Make to compile the code.

### Prerequisites

-   [ ] Make
-   [ ] OpenCV

<!-- Give references to install Make and OpenCV -->

### Installation

1. Clone the repo

```sh
git clone https://github.com/Kevin-Heleodoro/Augmented-Museum.git
```

2. Run Make from the root directory

```sh
make
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->

## Usage

1.  To compile the project using the `makefile` simply execute the `make` command with no parameters at the root of the project.
2.  To run the program execute `./main.exe`

```sh
Available parameters:

  -h	--help

   This parameter is optional. The default value is ''.

  -p	--path
   Set path for directory containing images. Defaults to ./paintings directory which contains a handful of assorted artworks.
   This parameter is optional. The default value is './paintings'.

  -c	--calibration
   Path to camera calibration file
   This parameter is optional. The default value is 'bin/calibration.xml'.

  -a	--aruco
   If true, creates an ArUco marker and saves it
   This parameter is optional. The default value is '0'.
```

3.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ROADMAP -->

## Roadmap

-   [x] ArUco Marker detection

-   ArUco markers can be detected by the application. Pose estimation calculated and frame axes drawn on the center of the marker.
    ![](img/README/aruco_detected_1.png)

-   [x] Placing image over ArUco Marker

-   Test image can be placed over ArUco marker. However the image sizing needs to be adjusted. The left border also sticks to the edge of the frame.
    ![](img/README/overlay_aruco_1.png)

-   I was incorrectly calculating new marker corners instead of using the known marker corners that were gathered during detection. Overlay image now covers ArUco marker and rotates/translates along with it.
    ![](img/README/overlay_aruco_2.png)

-   [ ] I want to paste the overlay image using the first ArUco marker corner while maintaining the original size of the image. The overlay should still rotate and translate with the ArUco marker, it will just be bigger.

    -   First attempt was able to match the first corners but the translation/rotation does not happen as expected. With this attempt I was using the difference between the starting points and the overlay's rows and columns. This did not account for the 3rd dimension translations.
        ![](img/README/overlay_aruco_3.png)

-   [ ] Application stops rendering overlay if the ArUco marker moves too quickly within the frame.

    -   Using `SOLVEPNP_ITERATIVE` which is faster at the expense of accuracy. Provides negligible differences to the eye.

-   [ ] Allow user to cycle through images.

See the [open issues](https://github.com/Kevin-Heleodoro/Augmented-Museum/issues) for a full list of proposed features (and known issues). -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

## License

Distributed under the MIT License.

<!-- See `LICENSE.txt` for more information. -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

Your Name - [@Golden_Sun_Kev](https://twitter.com/@Golden_Sun_Kev) - kevin.heleodoro@gmail.com

Project Link: [https://github.com/Kevin-Heleodoro/Augmented-Museum](https://github.com/Kevin-Heleodoro/Augmented-Museum)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->

## Acknowledgments

-   [Best Artwork of All Time Dataset](https://www.kaggle.com/datasets/ikarus777/best-artworks-of-all-time?resource=download-directory&select=resized)
-   [CmdParser Library](https://github.com/FlorianRappl/CmdParser)
<!-- -   [Blender](https://www.blender.org/features/) -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/Kevin-Heleodoro/Augmented-Museum.svg?style=for-the-badge
[contributors-url]: https://github.com/Kevin-Heleodoro/Augmented-Museum/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/Kevin-Heleodoro/Augmented-Museum.svg?style=for-the-badge
[forks-url]: https://github.com/Kevin-Heleodoro/Augmented-Museum/network/members
[stars-shield]: https://img.shields.io/github/stars/Kevin-Heleodoro/Augmented-Museum.svg?style=for-the-badge
[stars-url]: https://github.com/Kevin-Heleodoro/Augmented-Museum/stargazers
[issues-shield]: https://img.shields.io/github/issues/Kevin-Heleodoro/Augmented-Museum.svg?style=for-the-badge
[issues-url]: https://github.com/Kevin-Heleodoro/Augmented-Museum/issues
[license-shield]: https://img.shields.io/github/license/Kevin-Heleodoro/Augmented-Museum.svg?style=for-the-badge
[license-url]: https://github.com/Kevin-Heleodoro/Augmented-Museum/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/Kevin-Heleodoro
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com
