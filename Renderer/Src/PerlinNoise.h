#pragma once

#define HEIGHT 2048
#define WIDTH 2048
#define DEPTH 8
class CPerlinNoise
{
public:
	CPerlinNoise();
	~CPerlinNoise();
	float* m_PerlinNoise;
	void GenerateWhiteNoise();
	void GeneratePerlinNoise();

private:
	int m_Width;
	int m_Height;
	int m_OctaveCount;
	float** m_Noise;
	float*** m_SmoothNoise;
	float Interpolate(float x0, float x1, float alpha);
	void GenerateSmoothNoise(int octave);
};


CPerlinNoise::CPerlinNoise() : m_Width(2048), m_Height(2048), m_OctaveCount(8)
{
	m_Noise = new float*[HEIGHT];
	for (int i = 0; i < HEIGHT; ++i)
		m_Noise[i] = new float[WIDTH];
	
	m_PerlinNoise = new float[HEIGHT*WIDTH];



	m_SmoothNoise = new float**[HEIGHT];
	for (int i = 0; i < HEIGHT; ++i) {
		m_SmoothNoise[i] = new float*[WIDTH];

		for (int j = 0; j < WIDTH; ++j)
			m_SmoothNoise[i][j] = new float[DEPTH];
	}


	
}


CPerlinNoise::~CPerlinNoise()
{

	delete[] m_PerlinNoise;

	for (int i = 0; i < HEIGHT; ++i)
		delete[] m_Noise[i];
	delete[] m_Noise;

	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j)
			delete[] m_SmoothNoise[i][j];

		delete[] m_SmoothNoise[i];
	}
	delete[] m_SmoothNoise;
}


float CPerlinNoise::Interpolate(float x0, float x1, float alpha)
{
	return x0 * (1 - alpha) + alpha * x1;
}

void CPerlinNoise::GenerateWhiteNoise()
{
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_Noise[i][j] =static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100;
		}
	}

}


void CPerlinNoise::GenerateSmoothNoise(int octave)
{


	int samplePeriod = 1 << octave; // calculates 2 ^ k
	float sampleFrequency = 1.0f / samplePeriod;

	for (int i = 0; i < m_Width; i++)
	{
		//calculate the horizontal sampling indices
		int sample_i0 = (i / samplePeriod) * samplePeriod;
		int sample_i1 = (sample_i0 + samplePeriod) % m_Width; //wrap around
		float horizontal_blend = (i - sample_i0) * sampleFrequency;

		for (int j = 0; j < m_Height; j++)
		{
			//calculate the vertical sampling indices
			int sample_j0 = (j / samplePeriod) * samplePeriod;
			int sample_j1 = (sample_j0 + samplePeriod) % m_Height; //wrap around
			float vertical_blend = (j - sample_j0) * sampleFrequency;

			//blend the top two corners
			float top = Interpolate(m_Noise[sample_i0][sample_j0],
				m_Noise[sample_i1][sample_j0], horizontal_blend);

			//blend the bottom two corners
			float bottom = Interpolate(m_Noise[sample_i0][sample_j1],
				m_Noise[sample_i1][sample_j1], horizontal_blend);

			//final blend
			m_SmoothNoise[i][j][octave] = Interpolate(top, bottom, vertical_blend);
		}
	}

}


void CPerlinNoise::GeneratePerlinNoise()
{

	float persistance = 0.5f;

	//generate smooth noise
	for (int i = 0; i < m_OctaveCount; i++)
	{
		GenerateSmoothNoise(i);
	}

	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;

	//blend noise together
	for (int octave = m_OctaveCount - 1; octave >= 0; octave--)
	{
		amplitude *= persistance;
		totalAmplitude += amplitude;

		for (int i = 0; i < m_Width; i++)
		{
			for (int j = 0; j < m_Height; j++)
			{
				m_PerlinNoise[i*m_Width + j] = m_SmoothNoise[i][j][octave] * amplitude;
			}
		}
	}

	//normalisation
	for (int i = 0; i < m_Width; i++)
	{
		for (int j = 0; j < m_Height; j++)
		{
			m_PerlinNoise[i*m_Width + j] /= totalAmplitude;
		}
	}


}