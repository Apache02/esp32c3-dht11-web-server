<script>
	let t = NaN;
	let h = NaN;
	let loading = true;

	setInterval(async () => {
	  try {
        loading = true;
	    const response = await fetch("/sensors.json");
	    if (!response.ok) {
	      throw new Error(`Response status: ${response.status}`);
	    }


	    const json = await response.json();
			t = json.temperature;
			h = json.humidity;
	  } catch (error) {
	    console.error(error.message);
	  }
      loading = false;
	}, 1000);
</script>

<h1>DHT11 + wifi</h1>

<main>
	<div class="card" class:loading>
		<span class="label">temperature</span>
        <strong>{t}</strong> <span class="measure">&deg;C</span>
	</div>
	<div class="card" class:loading>
		<span class="label">humidity</span>
        <strong>{h}</strong><span class="measure">%</span>
	</div>
</main>

<style>
	main {
		display: flex;
		flex-wrap: wrap;
		margin: 2em;
		gap: 1em;
	}

	h1 {
      font-size: 3.2em;
      line-height: 1.1;
    }

	.card {
		flex: 1 2 100px;
		display: block;
		text-align: center;
		border: 1px solid #ccc;
		border-radius: 3px;
		padding: 2em;
		text-wrap: nowrap;
		transition: .25s background-color ease;
	}

	.card strong {
	    font-size: 2em;
	}

	.label {
	    display: block;
	    font-size: .75em;
	    color: #aaa;
	}

	.loading {
		background-color: #eee;
	}
</style>
