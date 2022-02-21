(async () => {
	const domLoading = document.querySelector('#loading');
	const domGameSelector = document.querySelector('#game-selector');
	const domResourceConverter = document.querySelector('#resource-converter');
	const domHm3 = document.querySelector('#hm3');
	const domHm2 = document.querySelector('#hm2');
	const domHm2016 = document.querySelector('#hm2016');
	const domHma = document.querySelector('#hma');
	const domBack = document.querySelector('#back');
	const domConvertJson = document.querySelector('#convert-json');
	const domGenerateResource = document.querySelector('#generate-resource');

	let selectedGame = null;

	let HM3 = null;
	let HM2 = null;
	let HM2016 = null;
	let HMA = null;

	const selectGame = async (game) => {
		game = game.toLowerCase();

		domGameSelector.style.display = 'none';
		domLoading.style.display = 'block';

		// Set the correct game module.
		if (game === 'hm3') {
			if (HM3 === null) {
				HM3 = await LoadHM3();
			}

			selectedGame = HM3;
		} else if (game === 'hm2') {
			if (HM2 === null) {
				HM2 = await LoadHM2();
			}
			
			selectedGame = HM2;
		} else if (game === 'hm2016') {
			if (HM2016 === null) {
				HM2016 = await LoadHM2016();
			}
			
			selectedGame = HM2016;
		} else if (game === 'hma') {
			if (HMA === null) {
				HMA = await LoadHMA();
			}
			
			selectedGame = HMA;
		} else {
			// Invalid game.
			return;
		}

		// Set game in URL.
		const newUrl = new URL(window.location);
		newUrl.hash = game;
		history.pushState({}, '', newUrl);

		// Show the converter dialog.
		domLoading.style.display = 'none';
		domGameSelector.style.display = 'none';
		domResourceConverter.style.display = 'block';
	};

	const goToGameSelection = () => {
		domResourceConverter.style.display = 'none';
		domGameSelector.style.display = 'block';

		const newUrl = new URL(window.location);
		newUrl.hash = '';
		history.pushState({}, '', newUrl);
	};

	const getFileFromEvent = (e) => {
		if (e.dataTransfer.items) {
			for (let i = 0; i < e.dataTransfer.items.length; ++i) {
				if (e.dataTransfer.items[i].kind === 'file') {
					return e.dataTransfer.items[i].getAsFile();
				}
			}
		} else {
			for (let i = 0; i < e.dataTransfer.files.length; ++i) {
				return ev.dataTransfer.files[i];
			}
		}

		return null;
	};

	const getFileExtension = (file) => {
		const fileNameParts = file.name.split('.');
		return fileNameParts[fileNameParts.length - 1];
	};

	const getSecondFileExtension = (file) => {
		const fileNameParts = file.name.split('.');

		if (fileNameParts.length < 3) {
			return null;
		}

		return fileNameParts[fileNameParts.length - 2];
	};

	const saveFile = (data, name) => {
		const blob = new Blob([ data ], { 
			type: 'application/octet-stream',
		});

		const url = URL.createObjectURL(blob);

		const link = document.createElement('a');
		link.style.display = 'none';
		link.href = url;
		link.download = name;

		document.body.appendChild(link);
		link.click();

		setTimeout(() => {
			document.body.removeChild(link);
			window.URL.revokeObjectURL(url);
		}, 0);
	};

	const convertToJson = (file) => {
		const fileName = file.name;
		const resourceType = getFileExtension(file).toUpperCase();

		if (!selectedGame.IsResourceTypeSupported(resourceType)) {
			console.error('Unsupported resource type: ' + resourceType);
			return;
		}

		// Show loading indicator.
		domResourceConverter.style.display = 'none';
		domLoading.style.display = 'block';

		file.arrayBuffer().then((buffer) => {
			const json = selectedGame.ConvertResourceToJson(resourceType, buffer);

			domLoading.style.display = 'none';
			domResourceConverter.style.display = 'block';

			if (json === null) {
				console.log('Failed to convert resource to json.');
				return;
			}

			saveFile(json, fileName + '.json');
		});
	};

	const convertFromJson = (file) => {
		const fileName = file.name;
		let resourceType = getSecondFileExtension(file);

		if (resourceType === null) {
			console.error('No resource type in file name.');
			return;
		}

		resourceType = resourceType.toUpperCase();

		if (getFileExtension(file).toLowerCase() != 'json') {
			console.error('Can only generate resources from json files.');
			return;
		}

		if (!selectedGame.IsResourceTypeSupported(resourceType)) {
			console.error('Unsupported resource type: ' + resourceType);
			return;
		}

		// Show loading indicator.
		domResourceConverter.style.display = 'none';
		domLoading.style.display = 'block';

		file.arrayBuffer().then((buffer) => {
			const resourceData = selectedGame.GenerateResourceFromJson(resourceType, buffer);

			domLoading.style.display = 'none';
			domResourceConverter.style.display = 'block';

			if (resourceData === null) {
				console.log('Failed to generate resource from json.');
				return;
			}

			saveFile(resourceData, fileName.substr(0, fileName.length - 5));
		});
	};

	// Button handlers.
	domHm3.addEventListener('click', (e) => {
		e.preventDefault();
		selectGame('HM3');
	});
	
	domHm2.addEventListener('click', (e) => {
		e.preventDefault();
		selectGame('HM2');
	});
	
	domHm2016.addEventListener('click', (e) => {
		e.preventDefault();
		selectGame('HM2016');
	});
	
	domHma.addEventListener('click', (e) => {
		e.preventDefault();
		selectGame('HMA');
	});
	
	domBack.addEventListener('click', (e) => {
		e.preventDefault();
		goToGameSelection();
	});

	// Resource to JSON handlers.
	domConvertJson.addEventListener('dragover', (e) => {
		e.preventDefault();
	});

	domConvertJson.addEventListener('drop', (e) => {
		e.preventDefault();
		domConvertJson.classList.remove('over');

		const file = getFileFromEvent(e);

		if (file === null) {
			return;
		}

		convertToJson(file);
	});

	domConvertJson.addEventListener('dragenter', (e) => {
		e.preventDefault();
		domConvertJson.classList.add('over');
	});

	domConvertJson.addEventListener('dragleave', (e) => {
		e.preventDefault();
		domConvertJson.classList.remove('over');
	});

	// JSON to Resource handlers.
	domGenerateResource.addEventListener('dragover', (e) => {
		e.preventDefault();
	});

	domGenerateResource.addEventListener('drop', (e) => {
		e.preventDefault();
		domGenerateResource.classList.remove('over');

		const file = getFileFromEvent(e);

		if (file === null) {
			return;
		}

		convertFromJson(file);
	});

	domGenerateResource.addEventListener('dragenter', (e) => {
		e.preventDefault();
		domGenerateResource.classList.add('over');
	});

	domGenerateResource.addEventListener('dragleave', (e) => {
		e.preventDefault();
		domGenerateResource.classList.remove('over');
	});

	// Show game selector.
	domLoading.style.display = 'none';
	domGameSelector.style.display = 'block';

	// Check if we already have a game specified in the URL.
	if (document.location.hash.length > 1) {
		selectGame(document.location.hash.substr(1));
	}
})();